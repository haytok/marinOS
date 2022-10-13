#include "defines.h"
#include "marinos.h"
#include "lib.h"

int test11_2_main(int argc, char *argv[])
{
	char *p;
	int size;

	puts("test11_2 started.\n");

	puts("test11_2 send in.\n");
	ma_send(MSGBOX_ID_MSGBOX1, 15, "static memory\n");
	puts("test11_2 send out.\n");

	p = ma_kmalloc(18);
	strcpy(p, "allocated memory\n");
	puts("test11_2 send in.\n");
	ma_send(MSGBOX_ID_MSGBOX1, 18, p);
	puts("test11_2 send out.\n");

	puts("test11_2 recv in.\n");
	ma_recv(MSGBOX_ID_MSGBOX2, &size, &p);
	puts("test11_2 recv out.\n");
	puts(p);

	puts("test11_2 recv in.\n");
	ma_recv(MSGBOX_ID_MSGBOX2, &size, &p);
	puts("test11_2 recv out.\n");
	puts(p);
	ma_kmfree(p);

	puts("test11_2 exit.\n");

	return 0;
}
