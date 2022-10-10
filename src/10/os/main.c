#include "defines.h"
#include "serial.h"
#include "lib.h"
#include "interrupt.h"
#include "intr.h"
#include "marinos.h"
#include "syscall.h"

int is_debug = 0;

static int start_threads(int argc, char *argv[])
{
	ma_run(test10_1_main, "test10_1", 1, 0x100, 0, NULL);

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
