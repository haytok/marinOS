#include "syscall.h"
#include "marinos.h"
#include "defines.h"
#include "lib.h"

// システムコール (スレッドを作成したいときに呼び出される。)
// start_threads から呼び出される。param.un.run.ret に応じたエラーハンドリングが不十分なので今後の課題とする。
ma_thread_id_t ma_run(ma_func_t func, char *name, int priority, int stacksize,
		      int argc, char *argv[])
{
	ma_syscall_param_t param;
	param.un.run.func = func;
	param.un.run.name = name;
	param.un.run.priority = priority;
	param.un.run.stacksize = stacksize;
	param.un.run.argc = argc;
	param.un.run.argv = argv;
	ma_syscall(MA_SYSCALL_TYPE_RUN, &param);
	return param.un.run.ret;
}

// システムコール
void ma_exit(void)
{
	ma_syscall(MA_SYSCALL_TYPE_EXIT, NULL);
}

// current のタスクを readyque の末尾に移動させる。
int ma_wait(void)
{
	ma_syscall_param_t param;
	ma_syscall(MA_SYSCALL_TYPE_WAIT, &param);
	return param.un.wait.ret;
}

// current のタスクを寝かす。
int ma_sleep(void)
{
	ma_syscall_param_t param;
	ma_syscall(MA_SYSCALL_TYPE_SLEEP, &param);
	return param.un.sleep.ret;
}

int ma_wakeup(ma_thread_id_t id)
{
	ma_syscall_param_t param;
	param.un.wakeup.id = id;
	ma_syscall(MA_SYSCALL_TYPE_WAKEUP, &param);
	return param.un.wakeup.ret;
}

ma_thread_id_t ma_getid(void)
{
	ma_syscall_param_t param;
	ma_syscall(MA_SYSCALL_TYPE_GETID, &param);
	return param.un.getid.ret;
}

int ma_chpri(int priority)
{
	ma_syscall_param_t param;
	param.un.chpri.priority = priority;
	ma_syscall(MA_SYSCALL_TYPE_CHPRI, &param);
	return param.un.chpri.ret;
}

// 各アプリケーションスレッド (ex. test10_1_main.c など) から呼び出される。
void *ma_kmalloc(int size)
{
	ma_syscall_param_t param;
	param.un.kmalloc.size = size;
	ma_syscall(MA_SYSCALL_TYPE_KMALLOC, &param);
	return param.un.kmalloc.ret;
}

int ma_kmfree(void *p)
{
	ma_syscall_param_t param;
	param.un.kmfree.p = p;
	ma_syscall(MA_SYSCALL_TYPE_KMFREE, &param);
	return param.un.kmfree.ret;
}

int ma_send(kz_msgbox_id_t id, int size, char *p)
{
	ma_syscall_param_t param;
	param.un.send.id = id; // 宛先メッセージボックス ID
	param.un.send.size = size;
	param.un.send.p = p;
	ma_syscall(MA_SYSCALL_TYPE_SEND, &param);
	return param.un.send.ret;
}

ma_thread_id_t ma_recv(kz_msgbox_id_t id, int *sizep, char **pp)
{
	ma_syscall_param_t param;
	// 受信側のスレッドがメッセージボックスから該当する検索するために必要なメッセージボックス ID
	param.un.recv.id = id;
	param.un.recv.sizep = sizep;
	param.un.recv.pp = pp;
	ma_syscall(MA_SYSCALL_TYPE_RECV, &param);
	return param.un.recv.ret;
}

int ma_setintr(softvec_type_t type, ma_handler_t handler)
{
	ma_syscall_param_t param;
	param.un.setintr.type = type;
	param.un.setintr.handler = handler;
	ma_syscall(MA_SYSCALL_TYPE_SETINTR, &param);
	return param.un.setintr.ret;
}

// サービスコールの実装 (サービスコールとは trap 命令を使用せずにシステムコールの処理をそのまま関数呼び出しするための関数)
// 完全には理解できていないけど、recv のサービスコールは待ち合わせが発生することがあるので、実装することは難しい。(ref p.446)
// 現時点では、例えば割り込みハンドラから呼び出されることを想定している。
int ms_wakeup(ma_thread_id_t id)
{
	ma_syscall_param_t param;
	param.un.wakeup.id = id;
	ma_srvcall(MA_SYSCALL_TYPE_WAKEUP, &param);
	return param.un.wakeup.ret;
}

void *ms_kmalloc(int size)
{
	ma_syscall_param_t param;
	param.un.kmalloc.size = size;
	ma_srvcall(MA_SYSCALL_TYPE_KMALLOC, &param);
	return param.un.kmalloc.ret;
}

int ms_kmfree(void *p)
{
	ma_syscall_param_t param;
	param.un.kmfree.p = p;
	ma_srvcall(MA_SYSCALL_TYPE_KMFREE, &param);
	return param.un.kmfree.ret;
}

int ms_send(kz_msgbox_id_t id, int size, char *p)
{
	ma_syscall_param_t param;
	param.un.send.id = id; // 宛先メッセージボックス ID
	param.un.send.size = size;
	param.un.send.p = p;
	ma_srvcall(MA_SYSCALL_TYPE_SEND, &param);
	return param.un.send.ret;
}
