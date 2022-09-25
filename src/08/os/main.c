#include "defines.h"
#include "serial.h"
#include "lib.h"
#include "interrupt.h"
#include "intr.h"

static void intr(softvec_type_t type, unsigned long sp)
{
	int c;
	static char buf[32];
	static int len = 0;

	c = getc();

	if (c != '\n') {
		buf[len++] = c;
	} else {
		buf[len++] = '\0';
		if (!strncmp(buf, "echo", 4)) {
			puts(buf + 4);
			PRINT_NEWLINE();
		} else {
			puts("unknown.\n");
		}
		puts("marinos> ");
		len = 0;
	}
}

int main(void)
{
	INTR_DISABLE;

	puts("Boot Succeeded.\n");
	puts("Started marinOS ...\n");
	puts("Hello World :)\n");

	softvec_setintr(SOFTVEC_TYPE_SERINTR, intr);
	serial_intr_recv_enable(SERIAL_DEFAULT_DEVICE);

	puts("marinos> ");

	INTR_ENABLE;

	while (1) {
		asm volatile("sleep");
	}

	return 0;
}
