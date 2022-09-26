#include "intr.h"
        .h8300h
        .section .text

        .global _intr_softerr
#       .type   _intr_softerr,@function
_intr_softerr:
        # 保存処理 (下方伸長)
        mov.l er6,@-er7
        mov.l er5,@-er7
        mov.l er4,@-er7
        mov.l er3,@-er7
        mov.l er2,@-er7
        mov.l er1,@-er7
        mov.l er0,@-er7 # この時点でレジスタの値をスタックに保存する処理が完了
        # 割り込みのために確保したスタックの領域を使用するように sp を変更する。
        # 旧 sp は割り込みのためのスタック領域の一番 top に保存する必要がある。
        # 割り込み処理
        mov.l er7,er1 # 旧 sp を er1 レジスタに保存
        mov.l #_intrstack,sp
        mov.l er1,@-er7 # 割り込み用のスタックの top に旧 sp を保存し、jsr の処理が完了したときに元の領域に戻れるようにする。
        mov.w #SOFTVEC_TYPE_SOFTERR,r0
        jsr   @_interrupt
        mov.l @er7+,er1
        mov.l er1,er7 # 旧 sp へ sp を移す。
        # 復帰処理
        # アドレスが大きい方に SP が進んでいく。
        # interrupt の処理が完了した時は er7 に元の SP の値に戻っているはずやけど、どこで戻るんやろか ...
        # 本章で実装する割り込みハンドラの実態の intr には er7 に sp を戻す処理は実装されてないけどな ...
        # interrupt の処理が完了してこの処理に戻ってきた時はすでに sp (er7) は interrupt が呼び出す前の状態に戻ってる？
        mov.l @er7+,er0
        mov.l @er7+,er1
        mov.l @er7+,er2
        mov.l @er7+,er3
        mov.l @er7+,er4
        mov.l @er7+,er5
        mov.l @er7+,er6
        rte

        .global _intr_syscall
#       .type   _intr_syscall,@function
_intr_syscall:
        mov.l er6,@-er7
        mov.l er5,@-er7
        mov.l er4,@-er7
        mov.l er3,@-er7
        mov.l er2,@-er7
        mov.l er1,@-er7
        mov.l er0,@-er7
        mov.l er7,er1
        mov.l #_intrstack,sp
        mov.l er1,@-er7
        mov.w #SOFTVEC_TYPE_SYSCALL,r0
        jsr @_interrupt
        mov.l @er7+,er1
        mov.l er1,er7
        # 復帰処理
        mov.l @er7+,er0
        mov.l @er7+,er1
        mov.l @er7+,er2
        mov.l @er7+,er3
        mov.l @er7+,er4
        mov.l @er7+,er5
        mov.l @er7+,er6
        rte

        .global _intr_serintr
#       .type   _intr_serintr,@function
_intr_serintr:
        mov.l er6,@-er7
        mov.l er5,@-er7
        mov.l er4,@-er7
        mov.l er3,@-er7
        mov.l er2,@-er7
        mov.l er1,@-er7
        mov.l er0,@-er7
        mov.l er7,er1
        mov.l #_intrstack,sp
        mov.l er1,@-er7
        mov.w #SOFTVEC_TYPE_SERINTR,r0
        jsr @_interrupt
        mov.l @er7+,er1
        mov.l er1,er7
        mov.l @er7+,er0
        mov.l @er7+,er1
        mov.l @er7+,er2
        mov.l @er7+,er3
        mov.l @er7+,er4
        mov.l @er7+,er5
        mov.l @er7+,er6
        rte
