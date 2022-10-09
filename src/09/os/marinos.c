#include "marinos.h"
#include "intr.h"
#include "defines.h"
#include "lib.h"
#include "interrupt.h"
#include "syscall.h"

#define THREAD_NUM 6
#define THREAD_NAME_SIZE 15

typedef struct _ma_context {
	uint_32 sp;
} ma_context;

typedef struct _ma_thread {
	struct _ma_thread *next;
	char name[THREAD_NAME_SIZE + 1];
	char *stack;

	struct {
		ma_func_t func;
		int argc;
		char **argv;
	} init;

	struct {
		ma_syscall_type_t type;
		ma_syscall_param_t *param;
	} syscall;

	ma_context context;
} ma_thread;

static struct {
	ma_thread *head;
	ma_thread *tail;
} readyque;

static ma_thread *current;
static ma_thread threads[THREAD_NUM];
static ma_handler_t handlers[SOFTVEC_TYPE_NUM];

void dispatch(ma_context *context);

static int getcurrent(void)
{
	if (current == NULL) {
		return -1;
	}

	// この時点では current が NULL でないことが保証されている。
	readyque.head = current->next;
	if (readyque.head == NULL) {
		readyque.tail = NULL;
	}
	// readyque から current は外れたが、current の next を NULL にしておく必要がある。
	// いらん操作な気がしなくもない ... -> いやいると思う。
	// head と tail は変更できたが、current->next を NULL にしておかないと、current から次の thread にアクセスできてしまう。
	current->next = NULL;

	return 0;
}

static int putcurrent(void)
{
	if (current == NULL) {
		return -1;
	}

	// current を末尾に追加する処理
	if (readyque.tail) {
		readyque.tail->next = current;
	} else {
		readyque.head = current;
	}
	// tail は定義上末尾の要素を指しているだけである。
	readyque.tail = current;

	return 0;
}

static void thread_end(void)
{
	// システムコールの呼び出し
	puts("[thread_end]\n");
	ma_exit();
}

static void thread_init(ma_thread *thp)
{
	puts("[thread_init] [0]");
	puts("\n");
	thp->init.func(thp->init.argc, thp->init.argv);
	// start_threads が return すると返ってくる。
	puts("[thread_init] [1]");
	puts("\n");
	thread_end();
}

static ma_thread_id_t thread_run(ma_func_t func, char *name, int stacksize,
				 int argc, char *argv[])
{
	int i;
	ma_thread *thp;
	uint_32 *sp;
	extern char userstack;
	// static にすることで変数のインクリメントが可能になる。
	static char *thread_stack = &userstack;

	// 空いている threads を見つける。
	for (i = 0; i < THREAD_NUM; i++) {
		thp = &threads[i];

		puts("[thread_run] tnp: ");
		putxval(thp, 0);
		puts("\n");

		// init があるかどうかで判定する。
		if (!thp->init.func) {
			break;
		}
	}
	if (i == THREAD_NUM) {
		return -1;
	}

	// thp を初期化
	memset(thp, 0, sizeof(*thp));

	// thp に引数として渡ってきた値に基づいて設定する。
	strcpy(thp->name, name);
	thp->next = NULL;

	thp->init.func = func;
	thp->init.argc = argc;
	thp->init.argv = argv;

	memset(thread_stack, 0, stacksize);
	thread_stack += stacksize;

	//
	thp->stack = thread_stack;
	sp = (uint_32 *)thp->stack;

	*(--sp) = (uint_32)thread_end;

	// PC と CCR を設定する。
	*(--sp) = (uint_32)thread_init;

	*(--sp) = 0; // ER6
	*(--sp) = 0; // ER5
	*(--sp) = 0; // ER4
	*(--sp) = 0; // ER3
	*(--sp) = 0; // ER2
	*(--sp) = 0; // ER1

	*(--sp) = (uint_32)thp; // dispatch する際に引き渡す引数

	thp->context.sp = (uint_32)sp; // 一番低位のアドレスが入っている。

	putcurrent();

	current = thp;
	putcurrent();

	puts("[thread_run]");
	putxval(sp, 0);
	puts("\n");

	return (ma_thread_id_t)current;
}

static int thread_exit(void)
{
	puts(current->name);
	puts("\n");
	puts(" [thread_exit] EXIT.\n");
	memset(current, 0, sizeof(*current));
	return 0;
}

// 割り込みハンドラの登録
static int setintr(softvec_type_t type, ma_handler_t handler)
{
	static void thread_intr(softvec_type_t type, unsigned long sp);

	softvec_setintr(type, thread_intr);

	handlers[type] = handler;

	return 0;
}

static void call_functions(ma_syscall_type_t type, ma_syscall_param_t *p)
{
	switch (type) {
	case MA_SYSCALL_TYPE_RUN:
		// *p のポインタを介して結果を呼び出し元に受け渡す。
		p->un.run.ret = thread_run(p->un.run.func, p->un.run.name,
					   p->un.run.stacksize, p->un.run.argc,
					   p->un.run.argv);
		break;
	case MA_SYSCALL_TYPE_EXIT:
		// *p のポインタを介して結果を呼び出し元に受け渡す。
		thread_exit();
		break;
	default:
		break;
	}
}

static void syscall_proc(ma_syscall_type_t type, ma_syscall_param_t *p)
{
	getcurrent();
	call_functions(type, p);
}

static void schedule(void)
{
	if (!readyque.head) {
		ma_sysdown();
	}

	current = readyque.head;
}

// システムコールの呼び出し。
// 呼び出しに関しては current に紐づいているオブジェクトから呼び出すような設計になっている。
static void syscall_intr(void)
{
	// グローバル変数を用いて syscall_proc を呼び出す。
	syscall_proc(current->syscall.type, current->syscall.param);
}

// ソフトウェアエラーの発生
static void softerr_intr(void)
{
	// syscall_intr とは違いこの時点でスレッドを殺す。
	puts(current->name);
	puts(" [softerr_intr] DOWN\n");
	getcurrent();
	thread_exit();
}

// 割り込みが発生したときに bootloader の interrupt 関数から呼び出されるハンドラ
static void thread_intr(softvec_type_t type, unsigned long sp)
{
	puts("[thread_intr] [0] current->name : ");
	puts(current->name);
	puts("\n");

	puts("[thread_intr] [00] sp : ");
	putxval(sp, 0);
	puts("\n");

	puts("[thread_intr] [00] &current->context : ");
	putxval(&current->context, 0);
	puts("\n");

	// 割り込みハンドラが実行される前の sp を context 変数に保持しておく。
	current->context.sp = sp;

	puts("[thread_intr] [000] &current->context : ");
	putxval(&current->context, 0);
	puts("\n");

	puts("[thread_intr] [000] current->context.sp : ");
	putxval(current->context.sp, 0);
	puts("\n");

	puts("[thread_intr] [000] &current->context.sp : ");
	putxval(&current->context.sp, 0);
	puts("\n");

	// handlers に登録されている関数 (syscall_intr, softerr_intr) が実行される。
	// この内部では最終的に thred_run が呼び出され、threads に作成されたスレッドが蓄積されていく。
	if (handlers[type]) {
		handlers[type]();
	}

	puts("[0000] [thread_intr] [after handlers[type]] &current->context : ");
	putxval(&current->context, 0);
	puts("\n");

	puts("[00000] [thread_intr] [before schedule()] current->name : ");
	puts(current->name);
	puts("\n");

	// なんでこれを実行するんかな ...
	// softerr_intr が上の handler の処理で実行されたときに、current が NULL になった時を想定してる？？？
	// それ以外の場合であれば current には何かしらの context は保持されていると思うけど ...
	schedule();

	// 2022/09/26 時点では fff4c0 が出力された
	puts("[1] [thread_intr] [after schedule()] current->name : ");
	puts(current->name);
	puts("\n");

	puts("[2] [thread_intr] [after schedule()] &current->context : ");
	putxval(&current->context, 0);
	puts("\n");

	dispatch(&current->context);
}

// ライブラリ関数
// main 関数から呼び出され、色々な初期化を実施する。
void ma_start(ma_func_t func, char *name, int stacksize, int argc, char *argv[])
{
	current = NULL;
	readyque.head = NULL;
	readyque.tail = NULL;
	memset(threads, 0, sizeof(threads));
	memset(handlers, 0, sizeof(handlers));

	setintr(SOFTVEC_TYPE_SYSCALL, syscall_intr);
	setintr(SOFTVEC_TYPE_SOFTERR, softerr_intr);

	current = (ma_thread *)thread_run(func, name, stacksize, argc, argv);

	puts("[ma_start] current->name : ");
	puts(current->name);
	puts("\n");

	dispatch(&current->context);

	puts("[ma_start] ...");
	puts("\n");
}

// ライブラリ関数
void ma_sysdown(void)
{
	puts("[ma_sysdown] System Error.\n");
	while (1)
		;
}

// ライブラリ関数
// システムコールの呼び出しはインラインアセンブラを使用して割り込みを直接発生させるような実装になっている。
void ma_syscall(ma_syscall_type_t type, ma_syscall_param_t *param)
{
	current->syscall.type = type;
	current->syscall.param = param;
	asm volatile("trapa #0");
}
