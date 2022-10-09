#include "syscall.h"
#include "marinos.h"
#include "defines.h"
#include "lib.h"

// システムコール (スレッドを作成したいときに呼び出される。)
// start_threads から呼び出される。param.un.run.ret に応じたエラーハンドリングが不十分なので今後の課題とする。
ma_thread_id_t ma_run(ma_func_t func, char *name, int stacksize, int argc,
		      char *argv[])
{
	ma_syscall_param_t param;
	param.un.run.func = func;
	param.un.run.name = name;
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
