OUTPUT_FORMAT("elf32-h8300")
OUTPUT_ARCH(h8300h)
/* _start は小文字じゃ無いとエラーになる。 */
ENTRY("_start")

MEMORY
{
    ramall(rwx) : o = 0xffbf20, l = 0x004000 /* 16KB */
    /* readelf marinos から確認できるエントリーポイントアドレスとなる。 */
    ram(rwx)    : o = 0xffc020, l = 0x003f00
    stack(rw)  : o = 0xffff00, l = 0x000000 /* RAM の末尾 */
}

SECTIONS
{
    .text : {
        _text_start = . ;
        *(.text)
        _etext = . ;
    } > ram

    .rodata : {
        _rodata_start = . ;
        *(.strings)
        *(.rodata)
        *(.rodata.*)
        _erodata = . ;
    } > ram
    /* 書き換えができへんデータを RAM に配置すると、書き換えれてしまう脆弱性ではないんかな ... */

    .data : {
        _data_start = . ;
        *(.data)
        _edata = . ;
    } > ram

    .bss : {
        _bss_start = . ;
        *(.bss)
        *(COMMON)
        _ebss = . ;
    } > ram

    . = ALIGN(4);
    _end = . ;

    .stack : {
        _stack = . ;
    } > stack
}
