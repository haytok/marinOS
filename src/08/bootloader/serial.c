#include "defines.h"
#include "serial.h"

#define SERIAL_SCI_NUM 3

#define H8_3069F_SCI0 ((volatile struct h8_3069f_sci *)0xffffb0)
#define H8_3069F_SCI1 ((volatile struct h8_3069f_sci *)0xffffb8)
#define H8_3069F_SCI2 ((volatile struct h8_3069f_sci *)0xffffc0)

// 各チャネルのレジスタの構成を構造体を使用して定義する。
struct h8_3069f_sci {
	volatile uint_8 smr;
	volatile uint_8 brr;
	volatile uint_8 scr;
	volatile uint_8 tdr;
	volatile uint_8 ssr;
	volatile uint_8 rdr;
	volatile uint_8 scmr;
};

// 以下の 3 つのレジスタしか定義してないけど、取り急ぎ必要やから定義した感じかな？？？
// 下位ビットから定義していくとわかりやすい。

// SMR (シリアル・モード・レジスタ) の各ビットを定義 (参考 仕様書 p.519)
// ビット 1 とビット 0 で取りうる値を定義
#define H8_3069F_SCI_SMR_CKS_PER1 (0 << 0)
#define H8_3069F_SCI_SMR_CKS_PER4 (1 << 0)
#define H8_3069F_SCI_SMR_CKS_PER16 (2 << 0)
#define H8_3069F_SCI_SMR_CKS_PER64 (3 << 0)
#define H8_3069F_SCI_SMR_MP (1 << 2)
#define H8_3069F_SCI_SMR_STOP (1 << 3)
#define H8_3069F_SCI_SMR_OE (1 << 4)
#define H8_3069F_SCI_SMR_PE (1 << 5)
#define H8_3069F_SCI_SMR_CHR (1 << 6)
#define H8_3069F_SCI_SMR_GM (1 << 7)
#define H8_3069F_SCI_SMR_CA (1 << 7)

// SCR (シリアル・コントロール・レジスタ) の各ビットを定義
#define H8_3069F_SCI_SCR_CKE0 (1 << 0)
#define H8_3069F_SCI_SCR_CKE1 (1 << 1)
#define H8_3069F_SCI_SCR_TEIE (1 << 2)
#define H8_3069F_SCI_SCR_MPIE (1 << 3)
#define H8_3069F_SCI_SCR_RE (1 << 4) /* 受信許可 */
#define H8_3069F_SCI_SCR_TE (1 << 5) /* 送信許可 */
#define H8_3069F_SCI_SCR_RIE (1 << 6) /* 受信割り込み許可 */
#define H8_3069F_SCI_SCR_TIE (1 << 7) /* 送信割り込み許可 */

// SSR (シリアル・ステータス・レジスタ) の各ビットを定義
#define H8_3069F_SCI_SSR_MPBT (1 << 0)
#define H8_3069F_SCI_SSR_MPB (1 << 1)
#define H8_3069F_SCI_SSR_TEND (1 << 2)
#define H8_3069F_SCI_SSR_PER (1 << 3)
#define H8_3069F_SCI_SSR_FERERS (1 << 4)
#define H8_3069F_SCI_SSR_ORER (1 << 5)
#define H8_3069F_SCI_SSR_RDRF (1 << 6) /* 受信完了 */
#define H8_3069F_SCI_SSR_TDRE (1 << 7) /* 送信完了 */

static struct {
	volatile struct h8_3069f_sci *sci; // 配列の 1 つの要素の要素になる。
} regs[SERIAL_SCI_NUM] = { { H8_3069F_SCI0 },
			   { H8_3069F_SCI1 },
			   { H8_3069F_SCI2 } };

// 上で定義した regs を使用してデバイスの初期化を行う。
int serial_init(int index)
{
	volatile struct h8_3069f_sci *sci = regs[index].sci;

	// どのレジスタを選択して設定するかはわかっていない。
	sci->scr = 0; // シリアル・コントロール・レジスタ
	sci->smr = 0; // シリアル・モード・レジスタ
	sci->brr = 64; // ビット・レート・レジスタ
	sci->scr = H8_3069F_SCI_SCR_RE |
		   H8_3069F_SCI_SCR_TE; // 送受信が可能な設定
	sci->ssr = 0; // シリアル・ステータス・レジスタ

	return 0;
}

int serial_is_send_enable(int index)
{
	volatile struct h8_3069f_sci *sci = regs[index].sci;
	return (sci->ssr & H8_3069F_SCI_SSR_TDRE);
}

int serial_send_byte(int index, unsigned char b)
{
	volatile struct h8_3069f_sci *sci = regs[index].sci;

	// 送信が可能になるまで Stay する。
	while (!serial_is_send_enable(index)) {
		;
	}

	// レジスタに送信したいデータをレジスタ TDR (トランスミット・データ・レジスタ) に書き込む。
	sci->tdr = b;
	// TDR に有効な値が設定されていて、データの送信が可能であることを示すために、SSR の TDRE のビットに 0 をセットする。(参考 p.526)
	sci->ssr &= ~H8_3069F_SCI_SSR_TDRE;

	return 0;
}

int serial_is_recv_enable(int index)
{
	volatile struct h8_3069f_sci *sci = regs[index].sci;
	return (sci->ssr & H8_3069F_SCI_SSR_RDRF);
}

unsigned char serial_recv_byte(int index)
{
	volatile struct h8_3069f_sci *sci = regs[index].sci;
	unsigned char c;

	while (!serial_is_recv_enable(index))
		;

	c = sci->rdr;
	sci->ssr &= ~H8_3069F_SCI_SSR_RDRF;

	return c;
}
