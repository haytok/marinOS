#ifndef _MARINOS_SYSCALL_H_INCLUDED_
#define _MARINOS_SYSCALL_H_INCLUDED_

#include "defines.h"

typedef enum {
	MA_SYSCALL_TYPE_RUN = 0,
	MA_SYSCALL_TYPE_EXIT,
	MA_SYSCALL_TYPE_WAIT,
	MA_SYSCALL_TYPE_SLEEP,
	MA_SYSCALL_TYPE_WAKEUP,
	MA_SYSCALL_TYPE_GETID,
	MA_SYSCALL_TYPE_CHPRI,
	MA_SYSCALL_TYPE_KMALLOC,
	MA_SYSCALL_TYPE_KMFREE,
} ma_syscall_type_t;

typedef struct {
	union {
		struct {
			ma_func_t func;
			char *name;
			int priority;
			int stacksize;
			int argc;
			char **argv;
			ma_thread_id_t ret;
		} run;
		struct {
			int dummy;
		} exit;
		struct {
			int ret;
		} wait;
		struct {
			int ret;
		} sleep;
		struct {
			ma_thread_id_t id;
			int ret;
		} wakeup;
		struct {
			ma_thread_id_t ret;
		} getid;
		struct {
			int priority;
			int ret;
		} chpri;
		struct {
			int size;
			void *ret;
		} kmalloc;
		struct {
			char *p;
			int ret;
		} kmfree;
	} un;
} ma_syscall_param_t;

#endif
