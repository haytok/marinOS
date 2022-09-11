#include "xmodem.h"
#include "lib.h"
#include "serial.h"
#include "defines.h"

// 制御コードの定義
#define XMODEM_SOH 0x01
#define XMODEM_STX 0x02
#define XMODEM_EOT 0x04
#define XMODEM_ACK 0x06
#define XMODEM_NAK 0x15
#define XMODEM_CAN 0x18
#define XMODEM_EOF 0x1a

// 今回の実装では、SOH の場合しか考慮していないので。ブロックサイズを決め打ちしている。
#define XMODEM_BLOCK_SIZE 128

// 受信準備完了し、受信が開始されるまでに NAK を返す関数
static int xmodem_wait(void)
{
	long cnt = 0;

	while (!serial_is_send_enable(SERIAL_DEFAULT_DEVICE)) {
		if (++cnt >= 2000000) {
			cnt = 0;
			// NAK が VAIO 側に届くまでに VAIO 側で送信が開始されてしまうと、いきなり VAIO 側は NAK を受け取ることになるので、バグる。
			serial_send_byte(SERIAL_DEFAULT_DEVICE, XMODEM_NAK);
		}
	}

	return 0;
}

// この関数が読み出される時点でパケットの先頭のなんのタイプかを識別するための情報を保持しているヘッダは読み出している。
// この関数で 1 ブロックの読み出しを実施する。
static int xmodem_read_block(unsigned char block_number, char *buf)
{
	unsigned char block_num, c, check_sum;
	int i;

	block_num = serial_recv_byte(SERIAL_DEFAULT_DEVICE);
	if (block_num != block_number) {
		return -1;
	}

	// データブロック番号の補数から、受信したデータが間違っていないかを検査する。
	block_num ^= serial_recv_byte(SERIAL_DEFAULT_DEVICE);
	if (block_num != 0xff) {
		return -1;
	}

	// 受信したデータは呼び出し元のバッファ buf に書き込む。
	check_sum = 0;
	for (i = 0; i < XMODEM_BLOCK_SIZE; i++) {
		c = serial_recv_byte(SERIAL_DEFAULT_DEVICE);
		*(buf++) = c;
		check_sum += c;
	}

	check_sum ^= serial_recv_byte(SERIAL_DEFAULT_DEVICE);
	if (check_sum) {
		return -1;
	}

	return i;
}

long xmodem_recv(char *buf)
{
	int r, receiving = 0;
	unsigned char c, block_number = 1;
	long size = 0;

	while (1) {
		// 一定時間が経つと、この if 文の処理を抜けて、その後の処理に移るはず。
		// それまでに VAIO 側から通信を開始する命令が飛んでくる前提のプログラムの設計となっている。
		if (!receiving) {
			xmodem_wait();
		}

		// getc ではなくドライバ (serial.c の関数) から直に値を読み出している。getc でも良い気がするが ...
		// エコーバックとかの処理も挟んでるから、あえて serial_recv_byte を読み出してるんかな？
		c = serial_recv_byte(SERIAL_DEFAULT_DEVICE);

		if (c == XMODEM_EOT) { // 終了パケットを受信
			serial_send_byte(SERIAL_DEFAULT_DEVICE, XMODEM_ACK);
			break;
		} else if (c == XMODEM_CAN) { // 中断
			return -1; // 負の値を返すときはエラーコード
		} else if (c == XMODEM_SOH) { // 受信開始
			receiving++;
			r = xmodem_read_block(block_number, buf);
			if (r < 0) { // エラーの場合は NAK を返す。
				serial_send_byte(SERIAL_DEFAULT_DEVICE,
						 XMODEM_NAK);
			} else { // 正常にデータを受信できた。
				block_number++;
				size += r;
				buf += r;
				serial_send_byte(SERIAL_DEFAULT_DEVICE,
						 XMODEM_ACK);
			}
		} else { // なんかの応答は返ってきている。
			if (receiving) {
				return -1; // 負の値を返すときはエラーコード
			}
		}
	}

	return size;
}
