#include "marinos.h"
#include "intr.h"
#include "defines.h"
#include "lib.h"
#include "interrupt.h"
#include "syscall.h"

#define THREAD_NUM 6
#define PRIORITY_NUM 16
#define THREAD_NAME_SIZE 15

typedef struct _ma_context {
	uint_32 sp;
} ma_context;

typedef struct _ma_thread {
	struct _ma_thread *next;
	char name[THREAD_NAME_SIZE + 1];
	int priority;
	char *stack;
	uint_32 flags;
#define MA_THREAD_FLAG_READY (1 << 0)

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
} readyque[PRIORITY_NUM];

static ma_thread *current;
static ma_thread threads[THREAD_NUM];
static ma_handler_t handlers[SOFTVEC_TYPE_NUM];

void dispatch(ma_context *context);

// current を readyque から取り出す関数
// 正常に処理が完了した時は 0 を返し、それ以外の場合は 1 or -1 を返す。
static int getcurrent(void)
{
	if (current == NULL) {
		return -1;
	}

	// flag を見て Ready な状態でなければ特に何もしない。
	if (!(current->flags & MA_THREAD_FLAG_READY)) {
		return 1;
	}

	// この時点では current が NULL でないことが保証されている。
	// priority に応じた操作を実装する。
	readyque[current->priority].head = current->next;
	if (readyque[current->priority].head == NULL) {
		readyque[current->priority].tail = NULL;
	}
	// readyque から current は外れたが、current の next を NULL にしておく必要がある。
	// いらん操作な気がしなくもない ... -> いやいると思う。
	// head と tail は変更できたが、current->next を NULL にしておかないと、current から次の thread にアクセスできてしまう。
	current->flags &= ~MA_THREAD_FLAG_READY;
	current->next = NULL;

	return 0;
}

static int putcurrent(void)
{
	if (current == NULL) {
		return -1;
	}

	// 対象の current が Ready な状態であれば特に何もしない。
	if (current->flags & MA_THREAD_FLAG_READY) {
		return 1;
	}

	// current を末尾に追加する処理
	if (readyque[current->priority].tail) {
		readyque[current->priority].tail->next = current;
	} else {
		readyque[current->priority].head = current;
	}
	// tail は定義上末尾の要素を指しているだけである。
	readyque[current->priority].tail = current;
	current->flags |= MA_THREAD_FLAG_READY;

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
	thp->init.func(thp->init.argc, thp->init.argv);
	thread_end();
}

static ma_thread_id_t thread_run(ma_func_t func, char *name, int priority,
				 int stacksize, int argc, char *argv[])
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
	thp->priority = priority;
	thp->flags = 0;

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
	*(--sp) = (uint_32)thread_init | ((uint_32)(priority ? 0 : 0xc0) << 24);

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

static int thread_wait(void)
{
	putcurrent();
	return 0;
}

// current->flags は特に操作せず、readyque から取り除くだけ
static int thread_sleep(void)
{
	return 0;
}

static int thread_wakeup(ma_thread_id_t id)
{
	DEBUG_CHAR("[0] [thread_wakeup] id ");
	DEBUG_XVAL(id, 0);
	DEBUG_NEWLINE();

	DEBUG_CHAR("[1] [thread_wakeup] current->name ");
	DEBUG_CHAR(current->name);
	DEBUG_NEWLINE();

	putcurrent();

	DEBUG_CHAR("[2] [thread_wakeup] current->name ");
	DEBUG_CHAR(current->name);
	DEBUG_NEWLINE();

	current = (ma_thread *)id;
	putcurrent();

	DEBUG_CHAR("[3] [thread_wakeup] current->name ");
	DEBUG_CHAR(current->name);
	DEBUG_NEWLINE();

	return 0;
}

static ma_thread_id_t thread_getid(void)
{
	putcurrent();

	return (ma_thread_id_t)current;
}

static int thread_chpri(int priority)
{
	DEBUG_CHAR("[0] [thread_chpri] current->name ");
	DEBUG_CHAR(current->name);
	DEBUG_NEWLINE();

	int old = current->priority;

	DEBUG_CHAR("[1] [thread_chpri] old ");
	DEBUG_XVAL(old, 0);
	DEBUG_NEWLINE();
	DEBUG_CHAR("[2] [thread_chpri] priority ");
	DEBUG_XVAL(priority, 0);
	DEBUG_NEWLINE();

	// priority に変な値が入っていないかのチェックを行う。
	// 書籍には入れていなかったが、上限のチェックも実装した。
	if (priority >= 0) {
		current->priority = priority;
	}

	DEBUG_CHAR("[4] [thread_chpri] current->priority ");
	DEBUG_XVAL(current->priority, 0);
	DEBUG_NEWLINE();

	putcurrent();

	DEBUG_CHAR("[5] [thread_chpri] current->name ");
	DEBUG_CHAR(current->name);
	DEBUG_NEWLINE();

	return old;
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
					   p->un.run.priority,
					   p->un.run.stacksize, p->un.run.argc,
					   p->un.run.argv);
		break;
	case MA_SYSCALL_TYPE_EXIT:
		// *p のポインタを介して結果を呼び出し元に受け渡す。
		thread_exit();
		break;
	case MA_SYSCALL_TYPE_WAIT:
		p->un.wait.ret = thread_wait();
		break;
	case MA_SYSCALL_TYPE_SLEEP:
		p->un.sleep.ret = thread_sleep();
		break;
	case MA_SYSCALL_TYPE_WAKEUP:
		DEBUG_CHAR("[0] [call_functions] ");
		DEBUG_XVAL(p->un.wakeup.id, 0);
		DEBUG_NEWLINE();

		p->un.wakeup.ret = thread_wakeup(p->un.wakeup.id);
		break;
	case MA_SYSCALL_TYPE_GETID:
		p->un.getid.ret = thread_getid();
		break;
	case MA_SYSCALL_TYPE_CHPRI:
		DEBUG_CHAR("[1] [call_functions] p->un.chpri.priority ");
		DEBUG_XVAL(p->un.chpri.priority, 0);
		DEBUG_NEWLINE();

		p->un.chpri.ret = thread_chpri(p->un.chpri.priority);

		DEBUG_CHAR("[2] [call_functions] p->un.chpri.ret ");
		DEBUG_XVAL(p->un.chpri.ret, 0);
		DEBUG_NEWLINE();

		break;
	default:
		break;
	}
}

static void syscall_proc(ma_syscall_type_t type, ma_syscall_param_t *p)
{
	getcurrent();

	DEBUG_CHAR("[0] [syscall_proc] p->un.wakeup.id ");
	DEBUG_XVAL(type, 0);
	DEBUG_NEWLINE();

	call_functions(type, p);

	DEBUG_CHAR("[1] [syscall_proc] p->un.wakeup.id ");
	DEBUG_XVAL(p->un.wakeup.id, 0);
	DEBUG_NEWLINE();
}

// 2022/10/10 はここから実装を再開する。
static void schedule(void)
{
	DEBUG_CHAR("[0] [schedule] current->name ");
	DEBUG_CHAR(current->name);
	DEBUG_NEWLINE();

	int i;

	for (i = 0; i < PRIORITY_NUM; i++) {
		if (readyque[i].head) {
			break;
		}
	}
	if (i == PRIORITY_NUM) {
		ma_sysdown();
	}

	DEBUG_CHAR("[1] [schedule] i ");
	DEBUG_XVAL(i, 0);
	DEBUG_NEWLINE();

	current = readyque[i].head;
}

// システムコールの呼び出し。
// 呼び出しに関しては current に紐づいているオブジェクトから呼び出すような設計になっている。
static void syscall_intr(void)
{
	// グローバル変数を用いて syscall_proc を呼び出す。
	syscall_proc(current->syscall.type, current->syscall.param);

	DEBUG_CHAR("[0] [syscall_intr] p->un.wakeup.id ");
	DEBUG_XVAL(current->syscall.param->un.wakeup.id, 0);
	DEBUG_NEWLINE();
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
	// 割り込みハンドラが実行される前の sp を context 変数に保持しておく。
	current->context.sp = sp;

	// handlers に登録されている関数 (syscall_intr, softerr_intr) が実行される。
	// この内部では最終的に thred_run が呼び出され、threads に作成されたスレッドが蓄積されていく。
	if (handlers[type]) {
		handlers[type]();
	}

	DEBUG_CHAR("[0] [thread_intr] current->name ");
	DEBUG_CHAR(current->name);
	DEBUG_NEWLINE();

	// なんでこれを実行するんかな ...
	// softerr_intr が上の handler の処理で実行されたときに、current が NULL になった時を想定してる？？？
	// それ以外の場合であれば current には何かしらの context は保持されていると思うけど ...
	schedule();

	DEBUG_CHAR("[1] [thread_intr] current->name ");
	DEBUG_CHAR(current->name);
	DEBUG_NEWLINE();

	dispatch(&current->context);
}

// ライブラリ関数
// main 関数から呼び出され、色々な初期化を実施する。
void ma_start(ma_func_t func, char *name, int priority, int stacksize, int argc,
	      char *argv[])
{
	current = NULL;
	memset(readyque, 0, sizeof(readyque));
	memset(threads, 0, sizeof(threads));
	memset(handlers, 0, sizeof(handlers));

	setintr(SOFTVEC_TYPE_SYSCALL, syscall_intr);
	setintr(SOFTVEC_TYPE_SOFTERR, softerr_intr);

	current = (ma_thread *)thread_run(func, name, priority, stacksize, argc,
					  argv);

	// puts("[ma_start] current->name : ");
	// puts(current->name);
	// puts("\n");

	DEBUG_CHAR("[ma_start] ...");
	DEBUG_NEWLINE();

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
