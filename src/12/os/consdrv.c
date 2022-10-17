#include "consdrv.h"
#include "defines.h"
#include "marinos.h"
#include "serial.h"
#include "intr.h"
#include "lib.h"
#include "interrupt.h"

#define CONS_BUFFER_SIZE 24

static struct consreg {
	ma_thread_id_t id; // コンソールの使用を依頼するスレッドの ID
	int index; // 利用する SCI の番号

	char *send_buf;
	char *recv_buf;
	int send_len;
	int recv_len;

	// padding 用
	long dummy[3];
} consreg[CONSDRV_DEVICE_NUM];

static int consdrv_init(void)
{
	memset(consreg, 0, sizeof(consreg));
	return 0;
}

// 実際にレジスタに送信したい文字を書き込む処理を実行している関数
static void send_char(struct consreg *cons)
{
	int i;

	serial_send_byte(cons->index, cons->send_buf[0]);
	cons->send_len--;

	for (i = 0; i < cons->send_len; i++) {
		cons->send_buf[i] = cons->send_buf[i + 1];
	}
}

static void send_string(struct consreg *cons, char *str, int len)
{
	int i;

	for (i = 0; i < len; i++) {
		// str は他のスレッドから受け取った文字列が格納されている。(send Data to SCI)
		if (str[i] == '\n') {
			cons->send_buf[cons->send_len++] = '\r';
		}
		cons->send_buf[cons->send_len++] = str[i];
	}

	// 割り込みが無効化されている状態の時にそのビットを立てることで有効化させ、割り込みができるようにする。
	if (cons->send_len && !serial_intr_is_send_enable(cons->index)) {
		serial_intr_send_enable(cons->index);
		// 1 文字だけ送信して、あとは割り込みハンドラに送信処理を任せる。
		send_char(cons);
	}
}

static int consdrv_command(struct consreg *cons, ma_thread_id_t id, int index,
			   int size, char *command)
{
	switch (command[0]) {
	case CONSDRV_CMD_USE:
		// 変数 consreg の初期化を実施
		cons->id = id;
		// SCI のインデックス (1) を計算して代入している。
		cons->index = command[1] - '0';
		cons->send_buf = ma_kmalloc(CONS_BUFFER_SIZE);
		cons->recv_buf = ma_kmalloc(CONS_BUFFER_SIZE * 2);
		cons->send_len = 0;
		cons->recv_len = 0;
		serial_init(cons->index);
		serial_intr_recv_enable(cons->index);
		break;
	case CONSDRV_CMD_WRITE:
		INTR_DISABLE;
		// USE の時と WRITE の時で command に入っている文字の種類が異なる ...
		// USE の時は command の 3 bit 目に SCI の index が入っているが、WRITE の時は 3 bit 目以降に送信したい文字列が格納されている。
		send_string(cons, command + 1, size - 1);
		INTR_ENABLE;
		break;
	default:
		break;
	}

	return 0;
}

static int consdrv_interproc(struct consreg *cons)
{
	unsigned char c;
	char *p;

	// 受信割り込み
	if (serial_is_recv_enable(cons->index)) {
		c = serial_recv_byte(cons->index);
		if (c == '\r') {
			c = '\n';
		}

		// エコーバック
		send_string(cons, &c, 1);

		if (cons->id) {
			if (c != '\n') {
				cons->recv_buf[cons->recv_len++] = c;
			} else {
				p = ms_kmalloc(CONS_BUFFER_SIZE);
				memcpy(p, cons->recv_buf, cons->recv_len);
				ms_send(MSGBOX_ID_CONSINPUT, cons->recv_len, p);
				cons->recv_len = 0;
			}
		}
	}

	// 送信割り込み
	// send_string() を呼び出したりすると実行される。
	if (serial_is_send_enable(cons->index)) {
		if (!cons->id || !cons->send_len) {
			// 送信できる文字列がなくなった。
			serial_intr_send_disable(cons->index);
		} else {
			send_char(cons);
		}
	}

	return 0;
}

// このファイル内における一番のメイン処理 (割り込み処理で実行される実体のハンドラ)
static void consdrv_intr(void)
{
	int i;
	struct consreg *cons;

	for (i = 0; i < CONSDRV_DEVICE_NUM; i++) {
		cons = &consreg[i];
		if (cons->id) {
			//
			if (serial_is_send_enable(cons->index) ||
			    serial_is_recv_enable(cons->index)) {
				consdrv_interproc(cons);
			}
		}
	}
}

// command スレッドよりも優先度が高い。
int consdrv_main(int argc, char *argv[])
{
	ma_thread_id_t id;
	int size, index;
	char *p;

	consdrv_init();
	ma_setintr(SOFTVEC_TYPE_SERINTR, consdrv_intr);

	while (1) {
		// 解析済みのクライアントから送信されたデータや送信したいデータをクライアント側に出力させる。
		id = ma_recv(MSGBOX_ID_CONSOUTPUT, &size, &p);

		// 受信した文字列の解析することで、consreg の index を取得する。('0' をベースに計算するのはこう言う取り決めやからっぽい。)
		index = p[0] - '0';
		consdrv_command(&consreg[index], id, index, size - 1, p + 1);

		ma_kmfree(p);
	}

	return 0;
}
