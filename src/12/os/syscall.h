#ifndef _MARINOS_SYSCALL_H_INCLUDED_
#define _MARINOS_SYSCALL_H_INCLUDED_

#include "defines.h"
#include "interrupt.h"

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
	MA_SYSCALL_TYPE_SEND,
	MA_SYSCALL_TYPE_RECV,
	MA_SYSCALL_TYPE_SETINTR,
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
		struct {
			kz_msgbox_id_t id;
			int size;
			char *p;
			int ret;
		} send;
		struct {
			kz_msgbox_id_t id;
			int *sizep;
			char **pp;
			kz_msgbox_id_t ret; // 送信側のスレッド ID が格納される。
		} recv;
		struct {
			softvec_type_t type;
			ma_handler_t handler;
			int ret;
		} setintr;
	} un;
} ma_syscall_param_t;

#endif
