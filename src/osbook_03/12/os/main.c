#include "defines.h"
#include "kozos.h"
#include "interrupt.h"
#include "lib.h"

/* �����ƥࡦ�������ȥ桼�����������ε�ư */
static int start_threads(int argc, char *argv[])
{
  kz_run(consdrv_main, "consdrv",  1, 0x200, 0, NULL);
  kz_run(command_main, "command",  8, 0x200, 0, NULL);

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
