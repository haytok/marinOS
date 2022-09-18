// そもそも glibc 等も自作するので、<> で include をしない。
#include "defines.h"
#include "serial.h"
#include "lib.h"
#include "xmodem.h"
#include "elf.h"

volatile int value_1;
volatile int value = 10;

// リンカスクリプト側で無駄にシンボルをつけてると思ってたのは、この init 関数から参照するためやったんか！
// この init 関数の呼び出しより前に静的変数を使用指定はいけない。
static int init(void)
{
	// exterm 修飾子を付けることで、リンカスクリプトのシンボルを参照することができる。
	extern int erodata, data_start, edata, bss_start, ebss;
	// 物理アドレスの .rodata セクションの直後に配置されている .data セクションのデータを RAM にコピーする
	memcpy(&data_start, &erodata, (long)&edata - (long)&data_start);
	// .bss の領域は 0 で初期化を実施する。(シンボル名のアドレスを & 演算子で取得する。)
	memset(&bss_start, 0, (long)&ebss - (long)&bss_start);

	// シリアルデバイスの初期化
	serial_init(SERIAL_DEFAULT_DEVICE);

	return 0;
}

static void wait()
{
	volatile long i;
	for (i = 0; i < 300000; i++)
		;
}

// メモリの 16 進ダンプ
static int dump(char *buf, long size)
{
	if (size < 0) {
		puts("no data :(\n");
		return -1;
	}

	long i;
	for (i = 0; i < size; i++) {
		putxval(buf[i], 2);
		if ((i & 0xf) == 15) {
			puts("\n");
		} else {
			// hexdump みたいに横に 16 個の 16 進数を出力するための処理を挟んでいる。
			if ((i & 0xf) == 7)
				puts(" ");
			puts(" ");
		}
	}
	puts("\n");

	return 0;
}

int main(void)
{
	unsigned char buf[16];
	static unsigned char *loadbuf = NULL;
	// VAIO 側から受け取ったデータを保存する RAM の領域のアドレスを保持する。
	extern int buffer_start;
	static long size = -1;

	// extern int data_start;
	// int *p;
	// p = &data_start;

	init();

	puts("marinos boot loader started ... :)\n");

	while (1) {
		// VAIO 側では以下の puts() 関数が実行されてから、処理の受け取り待ちの状態になる。
		puts("marinos> ");
		gets(buf);

		if (!strcmp(buf, "load")) {
			loadbuf = (char *)(&buffer_start);
			size = xmodem_recv(loadbuf);
			// 転送アプリケーションが終了して、端末アプリケーションに制御が戻るまで待ち合わせる。
			wait();
			if (size < 0) {
				puts("\nXMODEM receive error :(\n");
			} else {
				puts("\nXMODEM receive succeeded :)\n");
			}
		} else if (!strcmp(buf, "dump")) {
			puts("size: ");
			putxval(size, 0);
			puts("\n");
			dump(loadbuf, size);
		} else if (!strcmp(buf, "run")) {
			elf_load(loadbuf);
		} else if (strncmp(buf, "echo", 4) == 0) {
			puts(buf + 4);
			PRINT_NEWLINE();
		} else {
			puts("unknown.\n");
		}
	}

	return 0;
}
