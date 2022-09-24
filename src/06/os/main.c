#include "defines.h"
#include "serial.h"
#include "lib.h"

int main(void)
{
	static char buf[32];

	puts("Boot Succeeded.\n");
	puts("Started marinOS ...\n");
	puts("Hello World :)\n");

	while (1) {
		puts("marinos> ");
		gets(buf);

		if (!strncmp(buf, "echo", 4)) {
			puts(buf + 4);
			PRINT_NEWLINE();
		} else if (!strncmp(buf, "exit", 4)) {
			break;
		} else {
			puts("unknown.\n");
		}
	}

	return 0;
}
