#include "defines.h"
#include "serial.h"
#include "lib.h"
#include "interrupt.h"
#include "intr.h"
#include "marinos.h"
#include "syscall.h"

ma_thread_id_t test09_1_id;
ma_thread_id_t test09_2_id;
ma_thread_id_t test09_3_id;

int is_debug = 0;

static int start_threads(int argc, char *argv[])
{
	DEBUG_CHAR("[start_threads] Start!!!");
	DEBUG_NEWLINE();

	test09_1_id = ma_run(test09_1_main, "test09_1", 1, 0x100, 0, NULL);
	test09_2_id = ma_run(test09_2_main, "test09_2", 2, 0x100, 0, NULL);
	test09_3_id = ma_run(test09_3_main, "test09_3", 3, 0x100, 0, NULL);

	DEBUG_CHAR("[start_threads] thread ids ");
	DEBUG_XVAL(test09_1_id, 0);
	DEBUG_NEWLINE();
	DEBUG_XVAL(test09_2_id, 0);
	DEBUG_NEWLINE();
	DEBUG_XVAL(test09_3_id, 0);
	DEBUG_NEWLINE();

	// idle スレッドの優先度を一番下の 15 に変更する。
	ma_chpri(15);
	INTR_ENABLE;
	while (1) {
		asm volatile("sleep");
	}

	return 0;
}

int main(void)
{
	INTR_DISABLE;

	puts("Boot Succeeded.\n");
	puts("Started marinOS ...\n");
	puts("Hello World :)\n");

	// marinos.c で定義されている marinos.c
	ma_start(start_threads, "idle", 0, 0x100, 0, NULL);

	// ここには戻ってこない。

	return 0;
}
