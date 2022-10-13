#ifndef _DEFINES_H_INCLUDED_
#define _DEFINES_H_INCLUDED_

#define NULL (void *)0
#define SERIAL_DEFAULT_DEVICE 1

typedef unsigned char uint_8;
typedef unsigned short uint_16;
typedef unsigned long uint_32;

typedef uint_32 ma_thread_id_t;
typedef int (*ma_func_t)(int argc, char *argv[]);
typedef void (*ma_handler_t)(void);

// スレッド 1 とスレッド 2 がそれぞれ互いのメッセージボックスにメッセージを送信する。
typedef enum {
	MSGBOX_ID_MSGBOX1 = 0,
	MSGBOX_ID_MSGBOX2,
	MSGBOX_ID_NUM
} kz_msgbox_id_t;

#endif
