#include "defines.h"
#include "kozos.h"
#include "interrupt.h"
#include "lib.h"

/* �����ƥࡦ�������ȥ桼��������åɤε�ư */
static int start_threads(int argc, char *argv[])
{
  kz_run(test11_1_main, "test11_1",  1, 0x100, 0, NULL);
  kz_run(test11_2_main, "test11_2",  2, 0x100, 0, NULL);

  kz_chpri(15); /* ͥ���̤򲼤��ơ������ɥ륹��åɤ˰ܹԤ��� */
  INTR_ENABLE; /* �����ͭ���ˤ��� */
  while (1) {
    asm volatile ("sleep"); /* �����ϥ⡼�ɤ˰ܹ� */
  }

  return 0;
}

int main(void)
{
  INTR_DISABLE; /* �����̵���ˤ��� */

  puts("kozos boot succeed!\n");

  /* OS��ư��� */
  kz_start(start_threads, "idle", 0, 0x100, 0, NULL);
  /* �����ˤ���äƤ��ʤ� */

  return 0;
}
