#include "defines.h"
#include "lib.h"
#include "marinos.h"

int test08_2_main(int argc, char *argv[])
{
	static char buf[32];

	puts("[test08_2_main] Started.\n");

	while (1) {
		puts(">");

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

	puts("[test08_2_main] Exit.\n");

	return 0;
}
