#include "defines.h"
#include "marinos.h"
#include "lib.h"

int test11_1_main(int argc, char *argv[])
{
	char *p;
	int size;

	puts("test11_1 started.\n");

	puts("test11_1 recv in.\n");
	ma_recv(MSGBOX_ID_MSGBOX1, &size, &p);
	puts("test11_1 recv out.\n");
	puts(p);

	puts("test11_1 recv in.\n");
	ma_recv(MSGBOX_ID_MSGBOX1, &size, &p);
	puts("test11_1 recv out.\n");
	puts(p);
	ma_kmfree(p);

	puts("test11_1 send in.\n");
	ma_send(MSGBOX_ID_MSGBOX2, 15, "static memory\n");
	puts("test11_1 send out.\n");

	p = ma_kmalloc(18);
	strcpy(p, "allocated memory\n");
	puts("test11_1 send in.\n");
	ma_send(MSGBOX_ID_MSGBOX2, 18, p);
	puts("test11_1 send out.\n");

	puts("test11_1 exit.\n");

	return 0;
}
