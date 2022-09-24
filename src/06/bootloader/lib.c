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

unsigned char getc(void)
{
	unsigned char c = serial_recv_byte(SERIAL_DEFAULT_DEVICE);
	// そもそも、\r の文字列が PC 側から飛んでくることがあるので、そのケアを実装する。
	// 改行周りの挙動がイマイチわかっていない。
	c = (c == '\r') ? '\n' : c;
	// エコーバックの実装。マイコンは文字列を受け取っているだけで、その受け取った文字列を VAIO 側で出力させるには putc 関数を使用してエコーバックをする必要がある。
	// なので、この処理をコメントアウトすると、入力した文字列が何も出力されない気がするが ...
	// -> 合ってた、なんも出力されんかった！
	putc(c);
	return c;
}

int gets(unsigned char *buf)
{
	unsigned int c;
	int i = 0;
	do {
		c = getc();
		// 改行周りの挙動がイマイチわかっていない。
		if (c == '\n') {
			c = '\0';
		}
		buf[i++] = c;
	} while (c);
	return i - 1;
}

int putxval(unsigned long value, int column)
{
	// 現時点では固定で割り当てるしかない。malloc 等を実行できない。
	char buf[9];
	char *p;

	p = buf + sizeof(buf) - 1;
	*(p--) = '\0';

	if (!value && !column)
		column++;

	while (value || column) {
		*(p--) = "0123456789abcdef"[value & 0xf];
		value >>= 4;
		if (column)
			column--;
	}

	// + 1 をしないと先頭に変な文字列が入る。
	puts(p + 1);

	return 0;
}

// The memset() function fills the first n bytes of the memory area pointed to by s with the constant byte c.
void *memset(void *s, int c, long len)
{
	char *head;
	for (head = s; len > 0; len--) {
		*(head++) = c;
	}
	return head;
}

// dest には文字列リテラルを指定してはいけない。
void *memcpy(void *dest, const void *src, long n)
{
	// char 専用の関数になってるからマクロを使ってうまいこと他の方にも対応できるようにした方が良い。
	// dest と src と n の長さあるいは値によってバグが発生する可能性がある。
	char *d = dest;
	const char *s = src;
	for (; n > 0; n--) {
		*(d++) = *(s++);
	}
	return dest;
}

int memcmp(const void *s1, const void *s2, long n)
{
	// NULL チェックしても良さそう。
	if (n == 0) {
		return 0;
	}

	// char 専
	const char *t1 = s1;
	const char *t2 = s2;
	for (; n > 0; n--) {
		if (*t1 != *t2) {
			return (*t1 > *t2) ? 1 : -1;
		}
		t1++;
		t2++;
	}
	return 0;
}

// 書籍とは違う実装になっているが、手元の Ubuntu 22.04 で検証したところ、問題なさそう。
int strlen(const char *s)
{
	const char *head = s;
	int len = 0;
	while (*head++)
		len++;
	return len;
}

// 手元の Ubutntu 22.04 で動作させると、セグフォで動作しなかった。
// ひとまず書籍通りの実装で置いておく。
char *strcpy(char *dest, const char *src)
{
	char *d = dest;
	for (;; dest++, src++) {
		*dest = *src;
		if (!*src)
			break;
	}
	return d;
}

int strcmp(const char *s1, const char *s2)
{
	while (*s1 || *s2) {
		if (*s1 != *s2) {
			return (*s1 > *s2) ? 1 : -1;
		}
		*s1++;
		*s2++;
	}
	return 0;
}

int strncmp(const char *s1, const char *s2, int n)
{
	while ((*s1 || *s2) && n > 0) {
		if (*s1 != *s2) {
			return (*s1 > *s2) ? 1 : -1;
		}
		*s1++;
		*s2++;
		n--;
	}
	return 0;
}
