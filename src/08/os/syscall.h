#ifndef _MARINOS_SYSCALL_H_INCLUDED_
#define _MARINOS_SYSCALL_H_INCLUDED_

#include "defines.h"

typedef enum {
	MA_SYSCALL_TYPE_RUN = 0,
	MA_SYSCALL_TYPE_EXIT,
} ma_syscall_type_t;

typedef struct {
	union {
		struct {
			ma_func_t func;
			char *name;
			int stacksize;
			int argc;
			char **argv;
			ma_thread_id_t ret;
		} run;
		struct {
			int dummy;
		} exit;
	} un;
} ma_syscall_param_t;

#endif
