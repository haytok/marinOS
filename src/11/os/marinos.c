#include "marinos.h"
#include "intr.h"
#include "defines.h"
#include "lib.h"
#include "interrupt.h"
#include "syscall.h"
#include "memory.h"

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

// 送信側がこの型のオブジェクトを作成して msgboxes に投げる。
typedef struct _ma_msgbuf {
	struct _ma_msgbuf *next;
	ma_thread *sender;
	struct {
		int size;
		char *p;
	} param;
} ma_msgbuf;

typedef struct _ma_msgbox {
	// メッセージが送信される前に受信要求を投げた時に使用する変数
	// また、この変数を使用して送信されたメッセージの実体を受け渡す。
	ma_thread *receiver;
	ma_msgbuf *head;
	ma_msgbuf *tail;
	long dummy[1]; // これをコメントアウトしてもコンパイルが通った ...
} ma_msgbox;

static struct {
	ma_thread *head;
	ma_thread *tail;
} readyque[PRIORITY_NUM];

static ma_thread *current;
static ma_thread threads[THREAD_NUM];
static ma_handler_t handlers[SOFTVEC_TYPE_NUM];
static ma_msgbox msgboxes[MSGBOX_ID_NUM];

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
	putcurrent();

	current = (ma_thread *)id;
	putcurrent();

	return 0;
}

static ma_thread_id_t thread_getid(void)
{
	putcurrent();

	return (ma_thread_id_t)current;
}

static int thread_chpri(int priority)
{
	int old = current->priority;

	// priority に変な値が入っていないかのチェックを行う。
	// 書籍には入れていなかったが、上限のチェックも実装した。
	if (priority >= 0) {
		current->priority = priority;
	}

	putcurrent();

	return old;
}

// このレイヤーではスレッドの readuque のための操作を実施しなければならない。
// memory.c で定義したメモリ管理のためのメイン処理関数 mamem_alloc を呼び出す。
static void *thread_kmalloc(int size)
{
	putcurrent();
	return mamem_alloc(size);
}

static int thread_kmfree(void *p)
{
	mamem_free(p);
	putcurrent();
	return 0;
}

// sendmsg と recvmsg は thread_send と thread_recv の中からしか呼ばれないのでヘッダファイルにわざわざ定義していない。
// msgboxes の該当する ID の箇所に作成したオブジェクトを突っ込む
static void sendmsg(ma_msgbox *mboxp, ma_thread *thp, int size, char *p)
{
	ma_msgbuf *mp;

	mp = (ma_msgbuf *)mamem_alloc(sizeof(*mp));
	if (mp == NULL) {
		ma_sysdown();
	}

	mp->next = NULL;
	mp->sender = thp;
	mp->param.size = size;
	mp->param.p = p;
	if (mboxp->tail) {
		mboxp->tail->next = mp;
	} else {
		mboxp->head = mp;
	}
	mboxp->tail = mp;
}

// mboxp から送信されたデータを読み出す。
// つまり、この関数を呼び出すことで呼び出された受信のシステムコールの引数に送信されたデータを書き込むことができる。
// そればできるのは mboxp に receiver の変数が存在するからである。
static void recvmsg(ma_msgbox *mboxp)
{
	puts("[0] [recvmsg]");
	puts("\n");

	ma_msgbuf *mp;
	ma_syscall_param_t *p;

	// 必ず mboxp にはメッセージが存在している前提で処理を実装しても OK ???
	// mboxp から先頭のメッセージを取り出す。
	mp = mboxp->head;
	mboxp->head = mp->next;
	if (mboxp->head == NULL) {
		mboxp->tail = NULL;
	}
	// mp->next が NULL でも何かのポインタを挿していても mboxp の先頭のポインタは書き換えないといけない。
	mp->next = NULL;

	puts("[00] [recvmsg] mp->param.p ");
	puts(mp->param.p);
	puts("\n");
	puts("[000] [recvmsg] mp->param.size ");
	putxval(mp->param.size, 0);
	puts("[0000] [recvmsg] mp->sender ");
	putxval(mp->sender, 0);
	puts("\n");

	// 受信側は送信側が送信したデータを読み出す。
	p = mboxp->receiver->syscall.param;
	p->un.recv.ret = (ma_thread_id_t)mp->sender;
	if (p->un.recv.sizep) {
		*(p->un.recv.sizep) = mp->param.size;
	}
	if (p->un.recv.pp) {
		*(p->un.recv.pp) = mp->param.p;
	}
	mboxp->receiver = NULL;
	// ma_kmfree() を呼び出すとエラーになる
	mamem_free(mp);

	puts("[1] [recvmsg]");
	puts("\n");
}

// send と recv の呼び出し元
// sender と receiver は事前に送信先/受信先のメッセージボックス ID を知っておく必要がある。
// size or -1 を返す。
static int thread_send(kz_msgbox_id_t id, int size, char *p)
{
	ma_msgbox *mboxp = &msgboxes[id];

	putcurrent();
	sendmsg(mboxp, current, size, p);

	// 受信待ちのスレッドがあるかを判断する変数
	// つまり、送信より先に受信のリクエストが飛んでいたら、この変数にその受信要求をしたスレッドの ID が入っている。
	if (mboxp->receiver) {
		puts("[0] [thread_send]");
		puts("\n");
		// 寝ているスレッドを起こして受信をさせる。
		current = mboxp->receiver;

		puts("[1] [thread_send] current->name ");
		puts(current->name);
		puts("\n");

		recvmsg(mboxp);

		puts("[2] [thread_send]");
		puts("\n");
		putcurrent();
	} // そうでない時は特に何もせず、recv のシステコールが呼ばれた時に送信されたメッセージを読み出す。

	return size;
}

// 引数に sizep とか pp の変数はいらんくないか？？？
// 送信側のスレッドの ID を返す。なんでこの値を返すようにしているかは不明。
static ma_thread_id_t thread_recv(kz_msgbox_id_t id, int *sizep, char **pp)
{
	ma_msgbox *mboxp = &msgboxes[id];

	// 他にそのメッセージを受信しようとしているスレッドがいた時
	// この例外処理もどうなんや？？？
	if (mboxp->receiver) {
		ma_sysdown();
	}

	mboxp->receiver = current;

	// 送信されるより先に受信要求を送ってしまった場合
	if (mboxp->head == NULL) {
		// getcurrent() してから putcurrent() をしていないので、このスレッドは寝る。
		return -1;
	}

	recvmsg(mboxp);
	putcurrent();

	// recvmsg が実行された時点で ret に結果は代入されている。
	return current->syscall.param->un.recv.ret;
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
		p->un.wakeup.ret = thread_wakeup(p->un.wakeup.id);
		break;
	case MA_SYSCALL_TYPE_GETID:
		p->un.getid.ret = thread_getid();
		break;
	case MA_SYSCALL_TYPE_CHPRI:
		p->un.chpri.ret = thread_chpri(p->un.chpri.priority);
		break;
	case MA_SYSCALL_TYPE_KMALLOC:
		p->un.kmalloc.ret = thread_kmalloc(p->un.kmalloc.size);
		break;
	case MA_SYSCALL_TYPE_KMFREE:
		p->un.kmfree.ret = thread_kmfree(p->un.kmfree.p);
		break;
	case MA_SYSCALL_TYPE_SEND:
		p->un.send.ret = thread_send(p->un.send.id, p->un.send.size,
					     p->un.send.p);
		break;
	case MA_SYSCALL_TYPE_RECV:
		p->un.recv.ret = thread_recv(p->un.recv.id, p->un.recv.sizep,
					     p->un.recv.pp);
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

// 2022/10/10 はここから実装を再開する。
static void schedule(void)
{
	int i;

	for (i = 0; i < PRIORITY_NUM; i++) {
		if (readyque[i].head) {
			break;
		}
	}
	if (i == PRIORITY_NUM) {
		ma_sysdown();
	}

	current = readyque[i].head;
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
	// 割り込みハンドラが実行される前の sp を context 変数に保持しておく。
	current->context.sp = sp;

	// handlers に登録されている関数 (syscall_intr, softerr_intr) が実行される。
	// この内部では最終的に thred_run が呼び出され、threads に作成されたスレッドが蓄積されていく。
	if (handlers[type]) {
		handlers[type]();
	}

	// なんでこれを実行するんかな ...
	// softerr_intr が上の handler の処理で実行されたときに、current が NULL になった時を想定してる？？？
	// それ以外の場合であれば current には何かしらの context は保持されていると思うけど ...
	schedule();

	dispatch(&current->context);
}

// ライブラリ関数
// main 関数から呼び出され、色々な初期化を実施する。
void ma_start(ma_func_t func, char *name, int priority, int stacksize, int argc,
	      char *argv[])
{
	mamem_init();

	current = NULL;
	memset(readyque, 0, sizeof(readyque));
	memset(threads, 0, sizeof(threads));
	memset(handlers, 0, sizeof(handlers));
	memset(msgboxes, 0, sizeof(msgboxes));

	setintr(SOFTVEC_TYPE_SYSCALL, syscall_intr);
	setintr(SOFTVEC_TYPE_SOFTERR, softerr_intr);

	current = (ma_thread *)thread_run(func, name, priority, stacksize, argc,
					  argv);

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
