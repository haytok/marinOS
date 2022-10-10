#include "defines.h"
#include "marinos.h"
#include "lib.h"

int test09_2_main(int argc, char *argv[])
{
	puts("test09_2 started.\n");

	puts("test09_2 sleep in.\n");
	ma_sleep();
	puts("test09_2 sleep out.\n");

	puts("test09_2 chpri in.\n");
	ma_chpri(3);
	puts("test09_2 chpri out.\n");

	puts("test09_2 wait in.\n");
	ma_wait();
	puts("test09_2 wait out.\n");

	puts("test09_2 exit.\n");

	return 0;
}
