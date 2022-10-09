#ifndef _MARINOS_H_INCLUDED_
#define _MARINOS_H_INCLUDED_

#include "defines.h"
#include "syscall.h"

// システムコール
// ma_run は start_threads から呼び出される。
// name の変数はデバッグの時に役立つ。
ma_thread_id_t ma_run(ma_func_t func, char *name, int stacksize, int argc,
		      char *argv[]);
void ma_exit(void);

// ライブラリ関数
// ma_start は main 関数から呼び出される。
void ma_start(ma_func_t func, char *name, int stacksize, int argc,
	      char *argv[]);
void ma_sysdown(void);
void ma_syscall(ma_syscall_type_t type, ma_syscall_param_t *param);

int test08_1_main(int argc, char *argv[]);
int test08_2_main(int argc, char *argv[]);

#endif
