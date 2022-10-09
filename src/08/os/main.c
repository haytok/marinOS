#include "defines.h"
#include "serial.h"
#include "lib.h"
#include "interrupt.h"
#include "intr.h"
#include "marinos.h"
#include "syscall.h"

static int start_threads(int argc, char *argv[])
{
	puts("[start_threads] START\n");

	ma_run(test08_1_main, "command1", 0x100, 0, NULL);
	ma_run(test08_2_main, "command2", 0x100, 0, NULL);

	puts("[start_threads] END\n");

	return 0;
}

int main(void)
{
	INTR_DISABLE;

	puts("Boot Succeeded.\n");
	puts("Started marinOS ...\n");
	puts("Hello World :)\n");

	// marinos.c で定義されている marinos.c
	ma_start(start_threads, "start", 0x100, 0, NULL);

	puts("[main] END");
	puts("\n");

	return 0;
}
