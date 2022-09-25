/* OUTPUT_FORMAT("elf32-h8300") */
OUTPUT_ARCH(h8300h)
/* _start は小文字じゃ無いとエラーになる。 */
ENTRY("_start")

MEMORY
{
    /* ROM に配置するものと RAM に配置するものを区別して分ける。 */
    /* ROM に配置するのは、vectors と text である。 */
    romall(rx) : o = 0x000000, l = 0x080000
    vectors(r) : o = 0x000000, l = 0x000100
    rom(rx)    : o = 0x000100, l = 0x07ff00

    ramall(rwx) : o = 0xffbf20, l = 0x004000 /* 16KB */
    softvec(rw) : o = 0xffbf20, l = 0x000040 /* 64 Bytes */
    buffer(rwx) : o = 0xffdf20, l = 0x001d00 /* 8KB */
    data(rwx)   : o = 0xfffc20, l = 0x000300
    stack(rwx)  : o = 0xffff00, l = 0x000000 /* RAM の末尾 */
}

SECTIONS
{
    .vectors : {
        vector.o(.data)
    } > vectors

    .text : {
        _text_start = . ;
        *(.text)
        _etext = . ;
    } > rom

    .rodata : {
        _rodata_start = . ;
        *(.strings)
        *(.rodata)
        *(.rodata.*)
        _erodata = . ;
    } > rom

    .softvec : {
        _softvec = . ;
    } > softvec

    /* buffer セクションは MEMORY コマンドで定義された領域に配置される。 */
    /* この領域に VAIO 側から転送されたファイルの内容を配置する。 */
    .buffer : {
        _buffer_start = . ;
    } > buffer

    .data : {
        _data_start = . ;
        *(.data)
        _edata = . ;
    } > data AT> rom

    .bss : {
        _bss_start = . ;
        *(.bss)
        *(COMMON)
        _ebss = . ;
    } > data AT> rom

    . = ALIGN(4);
    _end = . ;

    .stack : {
        _stack = . ;
    } > stack
}
