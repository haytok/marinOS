#include "defines.h"
#include "kozos.h"
#include "intr.h"
#include "interrupt.h"
#include "serial.h"
#include "lib.h"
#include "consdrv.h"

#define CONS_BUFFER_SIZE 24

static struct consreg {
	kz_thread_id_t
		id; /* ���󥽡�������Ѥ��륹��å� */
	int index; /* ���Ѥ��륷�ꥢ����ֹ� */

	char *send_buf; /* �����Хåե� */
	char *recv_buf; /* �����Хåե� */
	int send_len; /* �����Хåե���Υǡ��������� */
	int recv_len; /* �����Хåե���Υǡ��������� */

	/* kozos.c �� kz_msgbox ��Ʊ�ͤ���ͳ�ǡ����ߡ������Фǥ�����Ĵ������ */
	long dummy[3];
} consreg[CONSDRV_DEVICE_NUM];

/*
 * �ʲ��Σ��Ĥδؿ�(send_char(), send_string())�ϳ���߽����ȥ���åɤ���
 * �ƤФ�뤬�����Хåե������Ƥ�������ԲĤΤ��ᡤ����åɤ���ƤӽФ�
 * ������¾�Τ������߶ػ߾��֤ǸƤ֤��ȡ�
 */

/* �����Хåե�����Ƭ��ʸ������������ */
static void send_char(struct consreg *cons)
{
	int i;
	serial_send_byte(cons->index, cons->send_buf[0]);
	cons->send_len--;
	/* ��Ƭʸ�������������Τǡ���ʸ���֤󤺤餹 */
	for (i = 0; i < cons->send_len; i++)
		cons->send_buf[i] = cons->send_buf[i + 1];
}

/* ʸ����������Хåե��˽񤭹����������Ϥ��� */
static void send_string(struct consreg *cons, char *str, int len)
{
	int i;
	for (i = 0; i < len; i++) { /* ʸ����������Хåե��˥��ԡ� */
		if (str[i] == '\n') /* \n��\r\n���Ѵ� */
			cons->send_buf[cons->send_len++] = '\r';
		cons->send_buf[cons->send_len++] = str[i];
	}
	/*
   * ���������̵���ʤ�С��������Ϥ���Ƥ��ʤ��Τ��������Ϥ��롥
   * ���������ͭ���ʤ���������Ϥ���Ƥ��ꡤ��������ߤα�Ĺ��
   * �����Хåե���Υǡ������缡���������Τǡ����⤷�ʤ��Ƥ褤��
   */
	if (cons->send_len && !serial_intr_is_send_enable(cons->index)) {
		serial_intr_send_enable(cons->index); /* ���������ͭ���� */
		send_char(cons); /* �������� */
	}
}

/*
 * �ʲ��ϳ���ߥϥ�ɥ餫��ƤФ�����߽����Ǥ��ꡤ��Ʊ����
 * �ƤФ��Τǡ��饤�֥��ؿ��ʤɤ�ƤӽФ����ˤ����դ�ɬ�ס�
 * ���ܤȤ��ơ��ʲ��Τ����줫�����ƤϤޤ�ؿ������ƤӽФ��ƤϤ����ʤ���
 * ��������ǽ�Ǥ��롥
 * ������åɤ���ƤФ�뤳�Ȥ�̵���ؿ��Ǥ��롥
 * ������åɤ���ƤФ�뤳�Ȥ����뤬������߶ػߤǸƤӽФ��Ƥ��롥
 * �ޤ��󥳥�ƥ����Ⱦ��֤ǸƤФ�뤿�ᡤ�����ƥࡦ����������Ѥ��ƤϤ����ʤ���
 * (�����ӥ�������������Ѥ��뤳��)
 */
static int consdrv_intrproc(struct consreg *cons)
{
	unsigned char c;
	char *p;

	if (serial_is_recv_enable(cons->index)) { /* ��������� */
		c = serial_recv_byte(cons->index);
		if (c == '\r') /* ���ԥ������Ѵ�(\r��\n) */
			c = '\n';

		send_string(cons, &c, 1); /* �������Хå����� */

		if (cons->id) {
			if (c != '\n') {
				/* ���ԤǤʤ��ʤ顤�����Хåե��˥Хåե���󥰤��� */
				cons->recv_buf[cons->recv_len++] = c;
			} else {
				/*
	 * Enter�������줿�顤�Хåե������Ƥ�
	 * ���ޥ�ɽ�������åɤ����Τ��롥
	 * (����ߥϥ�ɥ�ʤΤǡ������ӥ�������������Ѥ���)
	 */
				p = kx_kmalloc(CONS_BUFFER_SIZE);
				memcpy(p, cons->recv_buf, cons->recv_len);
				kx_send(MSGBOX_ID_CONSINPUT, cons->recv_len, p);
				cons->recv_len = 0;
			}
		}
	}

	if (serial_is_send_enable(cons->index)) { /* ��������� */
		if (!cons->id || !cons->send_len) {
			/* �����ǡ�����̵���ʤ�С�����������λ */
			serial_intr_send_disable(cons->index);
		} else {
			/* �����ǡ���������ʤ�С���³���������� */
			send_char(cons);
		}
	}

	return 0;
}

/* ����ߥϥ�ɥ� */
static void consdrv_intr(void)
{
	int i;
	struct consreg *cons;

	for (i = 0; i < CONSDRV_DEVICE_NUM; i++) {
		cons = &consreg[i];
		if (cons->id) {
			if (serial_is_send_enable(cons->index) ||
			    serial_is_recv_enable(cons->index))
				/* ����ߤ�����ʤ�С�����߽�����ƤӽФ� */
				consdrv_intrproc(cons);
		}
	}
}

static int consdrv_init(void)
{
	memset(consreg, 0, sizeof(consreg));
	return 0;
}

/* ����åɤ�����׵��������� */
static int consdrv_command(struct consreg *cons, kz_thread_id_t id, int index,
			   int size, char *command)
{
	switch (command[0]) {
	case CONSDRV_CMD_USE: /* ���󥽡��롦�ɥ饤�Фλ��ѳ��� */
		cons->id = id;
		cons->index = command[1] - '0';
		cons->send_buf = kz_kmalloc(CONS_BUFFER_SIZE);
		cons->recv_buf = kz_kmalloc(CONS_BUFFER_SIZE);
		cons->send_len = 0;
		cons->recv_len = 0;
		serial_init(cons->index);
		serial_intr_recv_enable(
			cons->index); /* ���������ͭ����(��������) */
		break;

	case CONSDRV_CMD_WRITE: /* ���󥽡���ؤ�ʸ������� */
		/*
     * send_string()�Ǥ������Хåե������Ƥ�������ԲĤʤΤǡ�
     * ��¾�Τ���˳���߶ػߤˤ��ƸƤӽФ���
     */
		INTR_DISABLE;
		send_string(cons, command + 1, size - 1); /* ʸ��������� */
		INTR_ENABLE;
		break;

	default:
		break;
	}

	return 0;
}

int consdrv_main(int argc, char *argv[])
{
	int size, index;
	kz_thread_id_t id;
	char *p;

	consdrv_init();
	kz_setintr(SOFTVEC_TYPE_SERINTR, consdrv_intr); /* ����ߥϥ�ɥ����� */

	while (1) {
		id = kz_recv(MSGBOX_ID_CONSOUTPUT, &size, &p);
		index = p[0] - '0';
		consdrv_command(&consreg[index], id, index, size - 1, p + 1);
		kz_kmfree(p);
	}

	return 0;
}
