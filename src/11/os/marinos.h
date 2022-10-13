#ifndef _MARINOS_H_INCLUDED_
#define _MARINOS_H_INCLUDED_

#include "defines.h"
#include "syscall.h"

// システムコール
// ma_run は start_threads から呼び出される。
// name の変数はデバッグの時に役立つ。
// thread_*** の中から呼び出される。
ma_thread_id_t ma_run(ma_func_t func, char *name, int priority, int stacksize,
		      int argc, char *argv[]);
void ma_exit(void);
int ma_wait(void); // current のタスクを readyque の末尾に移動させる。
int ma_sleep(void); // current のタスクを寝かす。
int ma_wakeup(ma_thread_id_t id);
ma_thread_id_t ma_getid(void);
int ma_chpri(int priority); // 古い優先度を返す。
void *ma_kmalloc(int size);
int ma_kmfree(void *p);
int ma_send(kz_msgbox_id_t id, int size, char *p);
ma_thread_id_t ma_recv(kz_msgbox_id_t id, int *sizep, char **pp);

// ライブラリ関数
// ma_start は main 関数から呼び出される。
void ma_start(ma_func_t func, char *name, int priority, int stacksize, int argc,
	      char *argv[]);
void ma_sysdown(void);
void ma_syscall(ma_syscall_type_t type, ma_syscall_param_t *param);

int test11_1_main(int argc, char *argv[]);
int test11_2_main(int argc, char *argv[]);

#endif
