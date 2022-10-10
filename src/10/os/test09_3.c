#include "defines.h"
#include "marinos.h"
#include "lib.h"

int test09_3_main(int argc, char *argv[])
{
	puts("test09_3 started.\n");

	puts("test09_3 wakeup in (test09_1).\n");
	ma_wakeup(test09_1_id);
	puts("test09_3 wakeup out.\n");

	puts("test09_3 wakeup in (test09_2).\n");
	ma_wakeup(test09_2_id);
	puts("test09_3 wakeup out.\n");

	puts("test09_3 wait in.\n");
	ma_wait();
	puts("test09_3 wait out.\n");

	puts("test09_3 exit in.\n");
	ma_exit();
	puts("test09_3 exit out.\n");

	return 0;
}
