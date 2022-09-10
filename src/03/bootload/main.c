// そもそも glibc 等も自作するので、<> で include をしない。
#include "defines.h"
#include "serial.h"
#include "lib.h"

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

int main(void)
{
	init();

	puts("Hello World :)\n");
	putxval(0x0a0, 3);
	PRINT_NEWLINE();

	// 検証 (変数 value が書き換えられないことを確認する)
	putxval(value, 0);
	PRINT_NEWLINE();
	value = 11;
	putxval(value, 0);

	while (1)
		;

	return 0;
}
