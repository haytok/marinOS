#include "defines.h"
#include "kozos.h"
#include "lib.h"

int test11_2_main(int argc, char *argv[])
{
  char *p;
  int size;

  puts("test11_2 started.\n");

  /* ��Ū�ΰ���å����������� */
  puts("test11_2 send in.\n");
  kz_send(MSGBOX_ID_MSGBOX1, 15, "static memory\n"); /* ���� */
  puts("test11_2 send out.\n");

  /* ưŪ�˳��������ΰ���å����������� */
  p = kz_kmalloc(18); /* ������� */
  strcpy(p, "allocated memory\n");
  puts("test11_2 send in.\n");
  kz_send(MSGBOX_ID_MSGBOX1, 18, p); /* ���� */
  puts("test11_2 send out.\n");
  /* ��������ϼ���¦�ǹԤ��Τǡ������Ǥ����� */

  /* ��Ū�ΰ���å������Ǽ��� */
  puts("test11_2 recv in.\n");
  kz_recv(MSGBOX_ID_MSGBOX2, &size, &p); /* ���� */
  puts("test11_2 recv out.\n");
  puts(p);

  /* ưŪ�˳��������ΰ���å������Ǽ��� */
  puts("test11_2 recv in.\n");
  kz_recv(MSGBOX_ID_MSGBOX2, &size, &p); /* ���� */
  puts("test11_2 recv out.\n");
  puts(p);
  kz_kmfree(p); /* ������� */

  puts("test11_2 exit.\n");

  return 0;
}
