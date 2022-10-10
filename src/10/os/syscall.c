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
	puts("=-=-=-=-=-=-=\n");
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
	DEBUG_CHAR("[0] [ma_wakeup] ");
	DEBUG_XVAL(id, 0);
	DEBUG_NEWLINE();

	ma_syscall_param_t param;
	param.un.wakeup.id = id;
	ma_syscall(MA_SYSCALL_TYPE_WAKEUP, &param);

	DEBUG_CHAR("[1] [ma_wakeup] ");
	DEBUG_XVAL(id, 0);
	DEBUG_NEWLINE();

	DEBUG_CHAR("[2] [ma_wakeup] ");
	DEBUG_XVAL(param.un.wakeup.ret, 0);
	DEBUG_NEWLINE();

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
