# 概要

- 5 章で実装した結果とそのプロセスにおいて気になった点を Log に残す。

## 本章で実装すること

- ELF 形式のフィアイルのプログラム・ヘッダ、テーブルを解析する処理を実装する。
- 実際に XMODEM プロトコルで ELF 形式のファイルをマイコンに転送し、実装した機能を確認する。

## 実装した結果

- ...

## 今後の課題

- [ ] ...

## メモ

- ELF ファイルの構造

![images/elf.png](images/elf.png)

```
Elf32_Addr 4 4 Unsigned program address
Elf32_Half 2 2 Unsigned medium integer
Elf32_Off 4 4 Unsigned file offset
Elf32_Sword 4 4 Signed large integer
Elf32_Word 4 4 Unsigned large integer
unsigned char 1 1 Unsigned small integer
```

- `kzload.elf` の構造 (readelf -a kzload.elf の出力結果)

```bash
ELF Header:
  Magic:   7f 45 4c 46 01 02 01 00 00 00 00 00 00 00 00 00
  Class:                             ELF32
  Data:                              2's complement, big endian
  Version:                           1 (current)
  OS/ABI:                            UNIX - System V
  ABI Version:                       0
  Type:                              EXEC (Executable file)
  Machine:                           Renesas H8/300
  Version:                           0x1
  Entry point address:               0x100
  Start of program headers:          52 (bytes into file)
  Start of section headers:          4020 (bytes into file)
  Flags:                             0x810000
  Size of this header:               52 (bytes)
  Size of program headers:           32 (bytes)
  Number of program headers:         3
  Size of section headers:           40 (bytes)
  Number of section headers:         9
  Section header string table index: 6

Section Headers:
  [Nr] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al
  [ 0]                   NULL            00000000 000000 000000 00      0   0  0
  [ 1] .vectors          PROGBITS        00000000 000094 000100 00  WA  0   0  4
  [ 2] .text             PROGBITS        00000100 000194 000d24 00  AX  0   0  2
  [ 3] .rodata           PROGBITS        00000e24 000eb8 0000a7 00   A  0   0  1
  [ 4] .data             PROGBITS        00fffc20 000f60 000014 00  WA  0   0  4
  [ 5] .bss              NOBITS          00fffc34 000f74 000006 00  WA  0   0  4
  [ 6] .shstrtab         STRTAB          00000000 000f74 00003d 00      0   0  1
  [ 7] .symtab           SYMTAB          00000000 00111c 0008e0 10      8 108  4
  [ 8] .strtab           STRTAB          00000000 0019fc 0002fb 00      0   0  1
Key to Flags:
  W (write), A (alloc), X (execute), M (merge), S (strings), I (info),
  L (link order), O (extra OS processing required), G (group), T (TLS),
  C (compressed), x (unknown), o (OS specific), E (exclude),
  D (mbind), p (processor specific)

There are no section groups in this file.

Program Headers:
  Type           Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align
  LOAD           0x000094 0x00000000 0x00000000 0x00100 0x00100 RW  0x1
  LOAD           0x000194 0x00000100 0x00000100 0x00dcb 0x00dcb R E 0x1
  LOAD           0x000f60 0x00fffc20 0x00000ecb 0x00014 0x0001a RW  0x1

 Section to Segment mapping:
  Segment Sections...
   00     .vectors 
   01     .text .rodata 
   02     .data .bss 

There is no dynamic section in this file.

There are no relocations in this file.

The decoding of unwind sections for machine type Renesas H8/300 is not currently supported.

Symbol table '.symtab' contains 142 entries:
   Num:    Value  Size Type    Bind   Vis      Ndx Name
     0: 00000000     0 NOTYPE  LOCAL  DEFAULT  UND 
     1: 00000000     0 SECTION LOCAL  DEFAULT    1 .vectors
     2: 00000100     0 SECTION LOCAL  DEFAULT    2 .text
     3: 00000e24     0 SECTION LOCAL  DEFAULT    3 .rodata
     4: 00fffc20     0 SECTION LOCAL  DEFAULT    4 .data
     5: 00fffc34     0 SECTION LOCAL  DEFAULT    5 .bss
     6: 00000000     0 FILE    LOCAL  DEFAULT  ABS vector.c
     7: 0000010a     0 NOTYPE  LOCAL  DEFAULT    2 .L1^B1
     8: 00000000     0 FILE    LOCAL  DEFAULT  ABS main.c
     9: 0000010c    76 NOTYPE  LOCAL  DEFAULT    2 _init
    10: 00000158    54 NOTYPE  LOCAL  DEFAULT    2 _wait
    11: 0000018e   234 NOTYPE  LOCAL  DEFAULT    2 _dump
    12: 00fffc34     4 OBJECT  LOCAL  DEFAULT    5 loadbuf___0
    13: 00fffc24     4 OBJECT  LOCAL  DEFAULT    4 size___1
    14: 00000186     0 NOTYPE  LOCAL  DEFAULT    2 .L2
    15: 00000168     0 NOTYPE  LOCAL  DEFAULT    2 .L3
    16: 000001c6     0 NOTYPE  LOCAL  DEFAULT    2 .L7
    17: 00000e24     0 NOTYPE  LOCAL  DEFAULT    3 .LC0
    18: 00000268     0 NOTYPE  LOCAL  DEFAULT    2 .L6
    19: 00000258     0 NOTYPE  LOCAL  DEFAULT    2 .L9
    20: 0000021e     0 NOTYPE  LOCAL  DEFAULT    2 .L11
    21: 00000e30     0 NOTYPE  LOCAL  DEFAULT    3 .LC1
    22: 00000246     0 NOTYPE  LOCAL  DEFAULT    2 .L10
    23: 0000023c     0 NOTYPE  LOCAL  DEFAULT    2 .L13
    24: 00000e32     0 NOTYPE  LOCAL  DEFAULT    3 .LC2
    25: 000001ce     0 NOTYPE  LOCAL  DEFAULT    2 .L8
    26: 00000e34     0 NOTYPE  LOCAL  DEFAULT    3 .LC3
    27: 00000e58     0 NOTYPE  LOCAL  DEFAULT    3 .LC4
    28: 00000e62     0 NOTYPE  LOCAL  DEFAULT    3 .LC5
    29: 00000310     0 NOTYPE  LOCAL  DEFAULT    2 .L17
    30: 00000304     0 NOTYPE  LOCAL  DEFAULT    2 .L18
    31: 00000e67     0 NOTYPE  LOCAL  DEFAULT    3 .LC6
    32: 00000292     0 NOTYPE  LOCAL  DEFAULT    2 .L15
    33: 00000e81     0 NOTYPE  LOCAL  DEFAULT    3 .LC7
    34: 00000e9f     0 NOTYPE  LOCAL  DEFAULT    3 .LC8
    35: 00000364     0 NOTYPE  LOCAL  DEFAULT    2 .L21
    36: 00000ea4     0 NOTYPE  LOCAL  DEFAULT    3 .LC9
    37: 00000eab     0 NOTYPE  LOCAL  DEFAULT    3 .LC10
    38: 000003a0     0 NOTYPE  LOCAL  DEFAULT    2 .L23
    39: 00000eb0     0 NOTYPE  LOCAL  DEFAULT    3 .LC11
    40: 00000000     0 FILE    LOCAL  DEFAULT  ABS lib.c
    41: 000003cc     0 NOTYPE  LOCAL  DEFAULT    2 .L2
    42: 00000414     0 NOTYPE  LOCAL  DEFAULT    2 .L5
    43: 000003f2     0 NOTYPE  LOCAL  DEFAULT    2 .L4
    44: 00000448     0 NOTYPE  LOCAL  DEFAULT    2 .L7
    45: 0000044e     0 NOTYPE  LOCAL  DEFAULT    2 .L8
    46: 000004a0     0 NOTYPE  LOCAL  DEFAULT    2 .L13
    47: 000004ca     0 NOTYPE  LOCAL  DEFAULT    2 .L11
    48: 00000484     0 NOTYPE  LOCAL  DEFAULT    2 .L10
    49: 00000538     0 NOTYPE  LOCAL  DEFAULT    2 .L16
    50: 0000054c     0 NOTYPE  LOCAL  DEFAULT    2 .L18
    51: 0000059e     0 NOTYPE  LOCAL  DEFAULT    2 .L17
    52: 00000eba     0 NOTYPE  LOCAL  DEFAULT    3 .LC0
    53: 0000061a     0 NOTYPE  LOCAL  DEFAULT    2 .L22
    54: 000005e4     0 NOTYPE  LOCAL  DEFAULT    2 .L21
    55: 000006ac     0 NOTYPE  LOCAL  DEFAULT    2 .L26
    56: 00000668     0 NOTYPE  LOCAL  DEFAULT    2 .L25
    57: 000006f6     0 NOTYPE  LOCAL  DEFAULT    2 .L29
    58: 00000790     0 NOTYPE  LOCAL  DEFAULT    2 .L28
    59: 0000078a     0 NOTYPE  LOCAL  DEFAULT    2 .L31
    60: 0000075e     0 NOTYPE  LOCAL  DEFAULT    2 .L33
    61: 0000074c     0 NOTYPE  LOCAL  DEFAULT    2 .L34
    62: 00000754     0 NOTYPE  LOCAL  DEFAULT    2 .L35
    63: 0000070e     0 NOTYPE  LOCAL  DEFAULT    2 .L30
    64: 000007e6     0 NOTYPE  LOCAL  DEFAULT    2 .L38
    65: 000007c4     0 NOTYPE  LOCAL  DEFAULT    2 .L37
    66: 0000083a     0 NOTYPE  LOCAL  DEFAULT    2 .L42
    67: 00000858     0 NOTYPE  LOCAL  DEFAULT    2 .L41
    68: 0000081c     0 NOTYPE  LOCAL  DEFAULT    2 .L40
    69: 0000089e     0 NOTYPE  LOCAL  DEFAULT    2 .L47
    70: 00000900     0 NOTYPE  LOCAL  DEFAULT    2 .L46
    71: 000008e2     0 NOTYPE  LOCAL  DEFAULT    2 .L48
    72: 000008d0     0 NOTYPE  LOCAL  DEFAULT    2 .L49
    73: 000008d8     0 NOTYPE  LOCAL  DEFAULT    2 .L50
    74: 00000906     0 NOTYPE  LOCAL  DEFAULT    2 .L44
    75: 00000884     0 NOTYPE  LOCAL  DEFAULT    2 .L45
    76: 0000094e     0 NOTYPE  LOCAL  DEFAULT    2 .L54
    77: 000009c6     0 NOTYPE  LOCAL  DEFAULT    2 .L53
    78: 0000099c     0 NOTYPE  LOCAL  DEFAULT    2 .L55
    79: 0000098a     0 NOTYPE  LOCAL  DEFAULT    2 .L56
    80: 00000992     0 NOTYPE  LOCAL  DEFAULT    2 .L57
    81: 000009cc     0 NOTYPE  LOCAL  DEFAULT    2 .L51
    82: 00000932     0 NOTYPE  LOCAL  DEFAULT    2 .L52
    83: 00000000     0 FILE    LOCAL  DEFAULT  ABS serial.c
    84: 00fffc28    12 OBJECT  LOCAL  DEFAULT    4 _regs
    85: 00000acc     0 NOTYPE  LOCAL  DEFAULT    2 .L5
    86: 00000abc     0 NOTYPE  LOCAL  DEFAULT    2 .L4
    87: 00000b7e     0 NOTYPE  LOCAL  DEFAULT    2 .L9
    88: 00000b6e     0 NOTYPE  LOCAL  DEFAULT    2 .L8
    89: 00000000     0 FILE    LOCAL  DEFAULT  ABS xmodem.c
    90: 00000bb6    84 NOTYPE  LOCAL  DEFAULT    2 _xmodem_wait
    91: 00000c0a   248 NOTYPE  LOCAL  DEFAULT    2 _xmodem_read_block
    92: 00000bfe     0 NOTYPE  LOCAL  DEFAULT    2 .L3
    93: 00000bc6     0 NOTYPE  LOCAL  DEFAULT    2 .L2
    94: 00000c46     0 NOTYPE  LOCAL  DEFAULT    2 .L6
    95: 00000cf2     0 NOTYPE  LOCAL  DEFAULT    2 .L5
    96: 00000c6e     0 NOTYPE  LOCAL  DEFAULT    2 .L7
    97: 00000cc4     0 NOTYPE  LOCAL  DEFAULT    2 .L9
    98: 00000c7a     0 NOTYPE  LOCAL  DEFAULT    2 .L8
    99: 00000cea     0 NOTYPE  LOCAL  DEFAULT    2 .L11
   100: 00000d34     0 NOTYPE  LOCAL  DEFAULT    2 .L15
   101: 00000d58     0 NOTYPE  LOCAL  DEFAULT    2 .L16
   102: 00000e06     0 NOTYPE  LOCAL  DEFAULT    2 .L14
   103: 00000d6e     0 NOTYPE  LOCAL  DEFAULT    2 .L18
   104: 00000e12     0 NOTYPE  LOCAL  DEFAULT    2 .L12
   105: 00000df0     0 NOTYPE  LOCAL  DEFAULT    2 .L20
   106: 00000dac     0 NOTYPE  LOCAL  DEFAULT    2 .L21
   107: 00000d28     0 NOTYPE  LOCAL  DEFAULT    2 .L13
   108: 00fffc38     2 OBJECT  GLOBAL DEFAULT    5 _value_1
   109: 0000086c   170 NOTYPE  GLOBAL DEFAULT    2 _strcmp
   110: 00000000   256 OBJECT  GLOBAL DEFAULT    1 _vectors
   111: 000003ae    54 NOTYPE  GLOBAL DEFAULT    2 _putc
   112: 00000e24     0 NOTYPE  GLOBAL DEFAULT    2 _etext
   113: 00000420    78 NOTYPE  GLOBAL DEFAULT    2 _getc
   114: 000003e4    60 NOTYPE  GLOBAL DEFAULT    2 _puts
   115: 00fffc20     2 OBJECT  GLOBAL DEFAULT    4 _value
   116: 00fffc34     0 NOTYPE  GLOBAL DEFAULT    5 _bss_start
   117: 00000100     0 NOTYPE  GLOBAL DEFAULT    2 _text_start
   118: 00000a4c    68 NOTYPE  GLOBAL DEFAULT    2 _serial_is_send_[...]
   119: 00000b44   114 NOTYPE  GLOBAL DEFAULT    2 _serial_recv_byte
   120: 0000046e   110 NOTYPE  GLOBAL DEFAULT    2 _gets
   121: 00fffc3a     0 NOTYPE  GLOBAL DEFAULT    5 _ebss
   122: 0000062e   150 NOTYPE  GLOBAL DEFAULT    2 _memcpy
   123: 000005bc   114 NOTYPE  GLOBAL DEFAULT    2 _memset
   124: 00000100     0 NOTYPE  GLOBAL DEFAULT    2 _start
   125: 00fffc20     0 NOTYPE  GLOBAL DEFAULT    4 _data_start
   126: 000009dc   112 NOTYPE  GLOBAL DEFAULT    2 _serial_init
   127: 000004dc   224 NOTYPE  GLOBAL DEFAULT    2 _putxval
   128: 000007f8   116 NOTYPE  GLOBAL DEFAULT    2 _strcpy
   129: 00ffdf20     0 NOTYPE  GLOBAL DEFAULT    4 _buffer_start
   130: 000006c4   220 NOTYPE  GLOBAL DEFAULT    2 _memcmp
   131: 00000916   198 NOTYPE  GLOBAL DEFAULT    2 _strncmp
   132: 00000e24     0 NOTYPE  GLOBAL DEFAULT    3 _rodata_start
   133: 00fffc34     0 NOTYPE  GLOBAL DEFAULT    4 _edata
   134: 00fffc3c     0 NOTYPE  GLOBAL DEFAULT  ABS _end
   135: 00000b00    68 NOTYPE  GLOBAL DEFAULT    2 _serial_is_recv_[...]
   136: 000007a0    88 NOTYPE  GLOBAL DEFAULT    2 _strlen
   137: 00000d02   290 NOTYPE  GLOBAL DEFAULT    2 _xmodem_recv
   138: 00000a90   112 NOTYPE  GLOBAL DEFAULT    2 _serial_send_byte
   139: 00ffff00     0 NOTYPE  GLOBAL DEFAULT    5 _stack
   140: 00000278   310 NOTYPE  GLOBAL DEFAULT    2 _main
   141: 00000ecb     0 NOTYPE  GLOBAL DEFAULT    3 _erodata

No version information found in this file.
```

```c
	struct {
		unsigned char magic[4];
		unsigned char class;
		unsigned char format;
		unsigned char version;
		unsigned char abi;
		unsigned char abi_version;
		unsigned char reserve[7];
	} id;
```

- 上の構造体の abi 以降は ELF の仕様書では EI_PAD になっていた。
- 書籍では abi とかを定義していたが、実際にこの領域の情報を使うことは仕様書からなさそうと感じた。
> EI_PAD 7 Start of padding bytes

- -> この引用からもわかるように EI_PAD は仕様書的には Padding の役割でしかないっぽい。

- 思想の違いからか、仕様書の構造体のフィールド名と書籍のフィールド名が異なる。正直、仕様書に命名規則を合わせたかったが、後々めんどくさいことになってもアレなので、書籍に命名規則を合わせることにした。
