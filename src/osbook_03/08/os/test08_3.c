#include "defines.h"
#include "kozos.h"
#include "lib.h"

int test08_3_main(int argc, char *argv[])
{
	char buf[32];

	puts("test08_3 started.\n");

	while (1) {
		puts("> ");
		gets(buf);

		if (!strncmp(buf, "echo", 4)) {
			puts(buf + 4);
			puts("\n");
		} else if (!strcmp(buf, "exit")) {
			break;
		} else {
			puts("unknown.\n");
		}
	}

	puts("test08_3 exit.\n");

	return 0;
}
