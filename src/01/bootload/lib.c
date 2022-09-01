// シリアル通信を介して 1 文字を送信する関数
// Ubuntu 22.04 on VAOI <--- 1 文字 --- H8/3069F
// つまり、serial 通信で文字列を送信する関数の実装が必要になる。
#include "defines.h"
#include "lib.h"
#include "serial.h"

int putc(unsigned char c)
{
	if (c == '\n') {
		// 第一引数でどのシリアルポートで通信を行うかを決める？ 0, 1, 2 のインデックスがあり、初期値は 1 になっている。
		serial_send_byte(SERIAL_DEFAULT_DEVICE, '\r');
	}
	return serial_send_byte(SERIAL_DEFAULT_DEVICE, c);
}

int puts(unsigned char *str)
{
	while (*str) {
		putc(*(str++));
	}
	return 0;
}
