#ifndef _KOZOS_H_INCLUDED_
#define _KOZOS_H_INCLUDED_

#include "defines.h"
#include "syscall.h"

/* �����ƥࡦ������ */
kz_thread_id_t kz_run(kz_func_t func, char *name, int stacksize, int argc,
		      char *argv[]);
void kz_exit(void);

/* �饤�֥��ؿ� */
void kz_start(kz_func_t func, char *name, int stacksize, int argc,
	      char *argv[]);
void kz_sysdown(void);
void kz_syscall(kz_syscall_type_t type, kz_syscall_param_t *param);

/* �桼��������å� */
int test08_1_main(int argc, char *argv[]);
int test08_2_main(int argc, char *argv[]);
int test08_3_main(int argc, char *argv[]);

#endif
