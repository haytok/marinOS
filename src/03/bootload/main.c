// そもそも glibc 等も自作するので、<> で include をしない。
#include "defines.h"
#include "serial.h"
#include "lib.h"

volatile int value = 10;

int main(void)
{
	// シリアルデバイスの初期化
	serial_init(SERIAL_DEFAULT_DEVICE);

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
