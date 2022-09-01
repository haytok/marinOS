// そもそも glibc 等も自作するので、<> で include をしない。
#include "defines.h"
#include "serial.h"
#include "lib.h"

int main(void)
{
	// シリアルデバイスの初期化
	serial_init(SERIAL_DEFAULT_DEVICE);

	puts("Hello World :)");

	while (1)
		;

	return 0;
}
