#include "marinos.h"
#include "consdrv.h"
#include "defines.h"
#include "lib.h"

// consddrv スレに初期化の処理を依頼
static void send_use(int index)
{
	char *p;
	int len;

	len = 3;
	p = ma_kmalloc(len);
	p[0] = '0';
	p[1] = CONSDRV_CMD_USE;
	p[2] = '0' + index;
	ma_send(MSGBOX_ID_CONSOUTPUT, len, p);
}

static void send_write(char *str)
{
	char *p;
	int len;

	len = strlen(str);
	p = ma_kmalloc(len + 2);

	p[0] = '0';
	p[1] = CONSDRV_CMD_WRITE;
	memcpy(&p[2], str, len);
	ma_send(MSGBOX_ID_CONSOUTPUT, len + 2, p);
}

int command_main(int argc, char *argv[])
{
	int size;
	char *p;

	send_use(SERIAL_DEFAULT_DEVICE);

	while (1) {
		// 書き込み先は msgbox の output の方
		send_write("command> ");

		ma_recv(MSGBOX_ID_CONSINPUT, &size, &p);
		p[size] = '\0';

		// 受信した文字列の解析処理
		if (!strncmp(p, "echo", 4)) {
			send_write(p + 4);
			send_write("\n");
		} else {
			send_write("unknown.\n");
		}

		ma_kmfree(p);
	}

	return 0;
}
