# 概要

- 5 章で実装した結果とそのプロセスにおいて気になった点を Log に残す。

## 本章で実装すること

- ELF 形式のフィアイルのプログラム・ヘッダ、テーブルを解析する処理を実装する。
- 実際に XMODEM プロトコルで ELF 形式のファイルをマイコンに転送し、実装した機能を確認する。

## 実装した結果

- 以下の出力結果を参照

## 今後の課題

- [ ] buffer で確保された領域以上のファイルが転送されたときはエラーメッセージと NAK を返すような実装に変更したい。

## メモ

- ELF ファイルの構造

![images/elf.png](images/elf.png)

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

- ELF の仕様書で定義されているビット幅

```
Elf32_Addr 4 4 Unsigned program address
Elf32_Half 2 2 Unsigned medium integer
Elf32_Off 4 4 Unsigned file offset
Elf32_Sword 4 4 Signed large integer
Elf32_Word 4 4 Unsigned large integer
unsigned char 1 1 Unsigned small integer
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

> e_phentsize This member holds the size in bytes of one entry in the file's program header table;
all entries are the same size.

- -> プログラムヘッダの全てのエントリは同じサイズであることが書かれている。

> The array element specifies a loadable segment, described by p_filesz and
p_memsz. The bytes from the file are mapped to the beginning of the memory
segment. If the segment's memory size (p_memsz) is larger than the file size
(p_filesz), the "extra'' bytes are defined to hold the value 0 and to follow the
segment's initialized area. The file size may not be larger than the memory size.
Loadable segment entries in the program header table appear in ascending order,
sorted on the p_vaddr member.

- -> Program Header の type が Load の状態、つまり 1 になっていると、Load 可能であることを表している。
- -> 逆に言うと、この type を確認すると、unused な領域なのか動的リンク情報を示しているかなどを確認することができる。

- 以下の int の箇所を long にするとエラーになった。原因はわからん。

```c
static int elf_load_program(struct elf_header *header)
{
	// long にするとエラーになった ... :(
	int i;
...
```

```bash
elf.o: In function `.L10':
elf.c:(.text+0x15b): undefined reference to `___mulsi3'
collect2: ld returned 1 exit status
make: *** [kzload] Error 1
```

- 4 章で転送したやり方をもとに、ELF ファイルを転送する。

```bash
send kzload.elf
```

- ファイルが転送できないエラーが発生していた。buffer のサイズは ld.scr で定義されているように 8KB である。ll でファイルサイズを確認すると以下のようになった。
  - buffer のサイズを超えると、エラーを返すメッセージと NAK を返す実装でも良いかもしれへん。

```bash
# CLFAGS += -mint32
# CFLAGS += -mint32
# -I : ヘッダファイルのの検索先を指定するためのオプション
CLFAGS += -I.
# CLFAGS += -g
CFLAGS += -I.
# CFLAGS += -g
# -Os : サイズ制約の最適化を行うためのオプション
CLFAGS += -Os
CLFAGS += -DKZLOAD
CFLAGS += -Os
CFLAGS += -DKZLOAD
```

- 最適化オプション等がないとき

```bash
[root@49136fab1f8c bootload]# ll | grep kzload.elf
-rwxr-xr-x 1 root root  8602 Sep 17 05:55 kzload.elf
```

- 最適化オプション等があるとき

```bash
[root@49136fab1f8c bootload]# ll | grep kzload.elf
-rwxr-xr-x 1 root root  5768 Sep 17 05:56 kzload.elf
```

- -> オプションのタイポにより、ファイルサイズが 8602 Byte から 5768 Byte に小さくすることができた。

- ELF ファイルのロードし、ダンプされた buffer 領域の解析することから、ELF ファイルが転送されていそうなのを確認する。

```bash
haytok@haytok-VJS132:~/workspace/marinOS/src/05/bootload$ kermit
Removing stale lock /var/lock/LCK..ttyUSB0 (pid 107685 terminated)
C-Kermit 9.0.305 OPEN SOURCE: Alpha.07, 24 Jan 2022, for Linux+SSL (64-bit)
 Copyright (C) 1985, 2022,
  Trustees of Columbia University in the City of New York.
Type ? or HELP for help.
(~/workspace/marinOS/src/05/bootload/) C-Kermit>c
Connecting to /dev/ttyUSB0, speed 9600
 Escape character: Ctrl-\ (ASCII 28, FS): enabled
Type the escape character followed by C to get back,
or followed by ? to see other options.
----------------------------------------------------

unknown.
marinos> load

(Back at haytok-VJS132)
----------------------------------------------------
(~/workspace/marinOS/src/05/bootload/) C-Kermit>send kzload.elf
Sending /home/haytok/workspace/marinOS/src/05/bootload/kzload.elf, 45 blocks: Give your local XMODEM receive command now.
Bytes Sent:   5888   BPS:88

Transfer complete
(~/workspace/marinOS/src/05/bootload/) C-Kermit>c
Connecting to /dev/ttyUSB0, speed 9600
 Escape character: Ctrl-\ (ASCII 28, FS): enabled
Type the escape character followed by C to get back,
or followed by ? to see other options.
----------------------------------------------------

XMODEM receive succeeded :)
marinos> dump
size: 1700
7f 45 4c 46 01 02 01 00  00 00 00 00 00 00 00 00
00 02 00 2e 00 00 00 01  00 00 01 00 00 00 00 34
00 00 0a c8 00 81 00 00  00 34 00 20 00 03 00 28
00 09 00 06 00 00 00 01  00 00 00 94 00 00 00 00
00 00 00 00 00 00 01 00  00 00 01 00 00 00 00 06
00 00 00 01 00 00 00 01  00 00 01 94 00 00 01 00
00 00 01 00 00 00 08 dd  00 00 08 dd 00 00 00 05
00 00 00 01 00 00 00 01  00 00 0a 74 00 ff fc 20
00 00 09 dd 00 00 00 14  00 00 00 1a 00 00 00 06
00 00 00 01 00 00 01 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 7a 07 00 ff  ff 00 5e 00 01 0c 40 fe
01 00 6d f6 0f f6 7a 17  ff ff ff e8 01 00 6d f4
01 00 6d f5 7a 02 00 ff  fc 34 7a 32 00 ff fc 20
7a 01 00 00 09 dd 7a 00  00 ff fc 20 5e 00 04 28
7a 02 00 ff fc 3a 7a 32  00 ff fc 34 19 11 7a 00
00 ff fc 34 5e 00 04 10  79 00 00 01 5e 00 05 38
7a 00 00 00 09 2e 5e 00  03 18 7a 00 00 00 09 52
5e 00 03 18 0f e4 7a 14  ff ff ff f0 0f c0 5e 00
03 6c 7a 01 00 00 09 5c  0f c0 5e 00 04 c4 0d 00
46 62 7a 00 00 ff df 20  01 00 6b a0 00 ff fc 34
5e 00 06 56 01 00 6b a0  00 ff fc 24 1a a2 01 00
6f e2 ff ec 01 00 6f 62  ff ec 7a 22 00 04 93 df
4e 14 0f e2 7a 12 ff ff  ff ec 01 00 69 23 0b 03
01 00 69 a3 40 de 01 00  6b 22 00 ff fc 24 4c 0a
7a 00 00 00 09 61 58 00  ff 7c 7a 00 00 00 09 7b
58 00 ff 72 7a 01 00 00  09 99 0f c0 5e 00 04 c4
0d 00 58 60 00 94 7a 00  00 00 09 9e 5e 00 03 18
19 11 01 00 6b 20 00 ff  fc 24 5e 00 03 ac 7a 00
00 00 09 af 5e 00 03 18  01 00 6b 22 00 ff fc 34
01 00 6f e2 ff e8 01 00  6b 24 00 ff fc 24 4c 0a
7a 00 00 00 09 a5 58 00  ff 1c 1a d5 1f c5 58 c0
00 88 01 00 6f 62 ff e8  0a d2 79 01 00 02 1a 80
68 28 5e 00 03 ac 1a a2  0c da ea 0f 7a 22 00 00
00 0f 46 08 7a 00 00 00  09 af 40 16 da 07 0f a2
46 0a 7a 00 00 00 09 db  5e 00 03 18 7a 00 00 00
09 db 5e 00 03 18 0b 05  40 b2 7a 01 00 00 09 b1
0f c0 5e 00 04 c4 0d 00  46 10 01 00 6b 20 00 ff
fc 34 5e 00 08 06 58 00  fe b0 79 02 00 04 7a 01
00 00 09 b5 0f c0 5e 00  04 f6 0d 00 46 16 0f e0
7a 10 ff ff ff f4 5e 00  03 18 7a 00 00 00 09 af
58 00 fe 82 7a 00 00 00  09 ba 58 00 fe 78 01 00
6d f6 0f f6 01 00 6d f4  0c 8c a8 0a 46 0a f9 0d
79 00 00 01 5e 00 05 92  0c c9 79 00 00 01 5e 00
05 92 01 00 6d 74 01 00  6d 76 54 70 01 00 6d f6
0f f6 01 00 6d f4 0f 84  68 4a 47 08 6c 48 5e 00
02 ea 40 f4 19 00 01 00  6d 74 01 00 6d 76 54 70
01 00 6d f6 0f f6 01 00  6d f4 79 00 00 01 5e 00
06 0c a8 0d 47 04 0c 8c  40 02 fc 0a 0c c8 5e 00
02 ea 0c c8 17 50 01 00  6d 74 01 00 6d 76 54 70
01 00 6d f6 0f f6 01 00  6d f4 01 00 6d f5 0f 85
19 44 5e 00 03 3c 17 50  79 20 00 0a 46 02 18 88
0d 42 17 f2 0a d2 68 a8  0b 04 0d 00 46 e4 0d 40
1b 00 01 00 6d 75 01 00  6d 74 01 00 6d 76 54 70
01 00 6d f6 0f f6 7a 17  ff ff ff f4 18 aa 6e ea
ff fc 0f e3 1b 93 1b 03  0f 80 46 06 0d 11 46 02
0b 01 1a a2 0c 8a ea 0f  78 20 6a 2a 00 00 09 c4
68 ba 1b 03 0f 82 11 32  11 32 11 32 11 32 0f a0
0d 11 47 02 1b 01 0f a2  46 d8 0d 11 46 d4 0f b0
0b 00 5e 00 03 18 19 00  7a 17 00 00 00 0c 01 00
6d 76 54 70 01 00 6d f6  0f f6 0f a2 4f 08 68 89
0b 00 1b 02 40 f4 01 00  6d 76 54 70 01 00 6d f6
0f f6 01 00 6d f4 0f 84  0f a3 0f b3 4f 0a 6c 1a
68 8a 0b 00 1b 03 40 f2  0f c0 01 00 6d 74 01 00
6d 76 54 70 01 00 6d f6  0f f6 01 00 6d f4 0f a2
47 24 0f 84 0f a2 4f 1e  68 48 68 1b 1c b8 47 0e
43 06 79 00 00 01 40 10  79 00 ff ff 40 0a 0b 04
0b 01 1b 02 40 de 19 00  01 00 6d 74 01 00 6d 76
54 70 01 00 6d f6 0f f6  19 33 6c 0a 47 04 0b 03
40 f8 0d 30 01 00 6d 76  54 70 01 00 6d f6 0f f6
0f 83 68 1a 68 8a 68 1a  47 06 0b 00 0b 01 40 f2
0f b0 01 00 6d 76 54 70  01 00 6d f6 0f f6 68 0a
46 04 68 1a 47 1c 68 0b  68 1a 1c ab 47 0e 43 06
79 00 00 01 40 0e 79 00  ff ff 40 08 0b 00 0b 01
40 dc 19 00 01 00 6d 76  54 70 01 00 6d f6 0f f6
01 00 6d f4 0f 84 0d 20  40 1a 68 1a 1c ab 47 0e
43 06 79 00 00 01 40 1a  79 00 ff ff 40 14 0b 04
0b 01 1b 00 68 4b 46 04  68 1a 47 04 0d 00 4e da
19 00 01 00 6d 74 01 00  6d 76 54 70 01 00 6d f6
0f f6 17 f0 10 30 10 30  01 00 78 00 6b 23 00 ff
fc 28 18 88 6e b8 00 02  68 b8 fa 40 6e ba 00 01
fa 30 6e ba 00 02 6e b8  00 04 19 00 01 00 6d 76
54 70 01 00 6d f6 0f f6  17 f0 10 30 10 30 01 00
78 00 6b 22 00 ff fc 28  6e 28 00 04 79 60 00 80
01 00 6d 76 54 70 01 00  6d f6 0f f6 1b 97 01 00
6d f4 01 00 6d f5 0d 05  6e e9 ff ff 0d 02 17 f2
10 32 10 32 01 00 78 20  6b 24 00 ff fc 28 0d 50
5e 00 05 6e 0d 00 47 f6  6e 6a ff ff 6e ca 00 03
6e 4a 00 04 ea 7f 6e ca  00 04 19 00 01 00 6d 75
01 00 6d 74 0b 97 01 00  6d 76 54 70 01 00 6d f6
0f f6 17 f0 10 30 10 30  01 00 78 00 6b 22 00 ff
fc 28 6e 28 00 04 79 60  00 40 01 00 6d 76 54 70
01 00 6d f6 0f f6 01 00  6d f4 01 00 6d f5 0d 05
0d 02 17 f2 10 32 10 32  01 00 78 20 6b 24 00 ff
fc 28 0d 50 5e 00 05 e8  0d 00 47 f6 6e 48 00 05
6e 4a 00 04 ea bf 6e ca  00 04 17 50 01 00 6d 75
01 00 6d 74 01 00 6d 76  54 70 01 00 6d f6 0f f6
7a 17 ff ff ff f0 01 00  6d f4 01 00 6d f5 01 00
6f e0 ff fc 19 22 6f e2  ff fa fb 01 6e eb ff f9
1a a2 01 00 6f e2 ff f4  6f 63 ff fa 46 26 1a c4
40 16 0b 04 7a 24 00 1e  84 7f 4f 0c 1a c4 f9 15
79 00 00 01 5e 00 05 92  79 00 00 01 5e 00 05 6e
0d 00 47 de 79 00 00 01  5e 00 06 0c a8 04 58 70
00 be a8 18 58 70 00 b2  a8 01 58 60 00 a4 6f 62
ff fa 0b 02 6f e2 ff fa  01 00 6f 64 ff fc 79 00
00 01 5e 00 06 0c 6e 6b  ff f9 1c b8 46 76 79 00
00 01 5e 00 06 0c 6e 6a  ff f9 15 8a aa ff 46 64
18 aa 6e ea ff f3 79 05  00 7f 79 00 00 01 5e 00
06 0c 68 c8 0b 04 6e 6b  ff f3 08 8b 6e eb ff f3
1b 05 0d 55 4c e4 79 00  00 01 5e 00 06 0c 6e 6a
ff f3 1c 8a 46 2e 6e 6b  ff f9 8b 01 6e eb ff f9
1a a2 8a 80 01 00 6f 63  ff f4 0a a3 01 00 6f e3
ff f4 01 00 6f 63 ff fc  0a a3 01 00 6f e3 ff fc
f9 06 40 02 f9 15 79 00  00 01 5e 00 05 92 58 00
ff 16 6f 62 ff fa 58 70  ff 0e 1a 80 1b 00 40 10
f9 06 79 00 00 01 5e 00  05 92 01 00 6f 60 ff f4
01 00 6d 75 01 00 6d 74  7a 17 00 00 00 10 01 00
6d 76 54 70 01 00 6d f6  0f f6 01 00 6d f4 0f 84
1a a2 8a 04 7a 01 00 00  09 d5 5e 00 04 50 0d 00
46 36 6e 4a 00 04 aa 01  46 2e 6e 4a 00 05 aa 02
46 26 6e 4a 00 06 aa 01  46 1e 6f 42 00 10 1b d2
46 16 01 00 6f 42 00 14  1b 72 46 0c 6f 42 00 12
79 12 ff d2 11 92 47 06  79 00 ff ff 40 02 19 00
01 00 6d 74 01 00 6d 76  54 70 01 00 6d f6 0f f6
1b 97 01 00 6d f4 01 00  6d f5 0f 85 5e 00 07 a0
0d 00 4c 08 79 00 ff ff  58 00 00 f6 19 22 6f e2
ff fe 6f 52 00 2c 19 33  1d 23 58 c0 00 e2 01 00
6f 52 00 1c 0f d4 0a a4  6f 52 00 2a 6f 63 ff fe
01 c0 52 32 17 f2 0a a4  01 00 69 42 1b 72 58 60
00 a8 79 01 00 06 01 00  6f 40 00 04 5e 00 03 ac
7a 00 00 00 09 da 5e 00  03 18 79 01 00 08 01 00
6f 40 00 08 5e 00 03 ac  7a 00 00 00 09 da 5e 00
03 18 79 01 00 08 01 00  6f 40 00 0c 5e 00 03 ac
7a 00 00 00 09 da 5e 00  03 18 79 01 00 05 01 00
6f 40 00 10 5e 00 03 ac  7a 00 00 00 09 da 5e 00
03 18 79 01 00 05 01 00  6f 40 00 14 5e 00 03 ac
7a 00 00 00 09 da 5e 00  03 18 79 01 00 02 01 00
6f 40 00 18 5e 00 03 ac  7a 00 00 00 09 da 5e 00
03 18 79 01 00 02 01 00  6f 40 00 1c 5e 00 03 ac
7a 00 00 00 09 da 5e 00  03 18 6f 62 ff fe 0b 02
6f e2 ff fe 6f 52 00 2c  6f 63 ff fe 58 00 ff 18
19 00 01 00 6d 75 01 00  6d 74 0b 97 01 00 6d 76
54 70 6d 61 72 69 6e 6f  73 20 62 6f 6f 74 20 6c
6f 61 64 65 72 20 73 74  61 72 74 65 64 20 2e 2e
2e 20 3a 29 0a 00 6d 61  72 69 6e 6f 73 3e 20 00
6c 6f 61 64 00 0a 58 4d  4f 44 45 4d 20 72 65 63
65 69 76 65 20 65 72 72  6f 72 20 3a 28 0a 00 0a
58 4d 4f 44 45 4d 20 72  65 63 65 69 76 65 20 73
75 63 63 65 65 64 65 64  20 3a 29 0a 00 64 75 6d
70 00 73 69 7a 65 3a 20  00 6e 6f 20 64 61 74 61
20 3a 28 0a 00 72 75 6e  00 65 63 68 6f 00 75 6e
6b 6e 6f 77 6e 2e 0a 00  30 31 32 33 34 35 36 37
38 39 61 62 63 64 65 66  00 7f 45 4c 46 00 20 20
00 00 00 00 00 0a 00 00  ff ff ff ff 00 ff ff b0
00 ff ff b8 00 ff ff c0  00 2e 73 79 6d 74 61 62
00 2e 73 74 72 74 61 62  00 2e 73 68 73 74 72 74
61 62 00 2e 76 65 63 74  6f 72 73 00 2e 74 65 78
74 00 2e 72 6f 64 61 74  61 00 2e 64 61 74 61 00
2e 62 73 73 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 1b 00 00 00 01  00 00 00 03 00 00 00 00
00 00 00 94 00 00 01 00  00 00 00 00 00 00 00 00
00 00 00 04 00 00 00 00  00 00 00 24 00 00 00 01
00 00 00 06 00 00 01 00  00 00 01 94 00 00 08 2e
00 00 00 00 00 00 00 00  00 00 00 02 00 00 00 00
00 00 00 2a 00 00 00 01  00 00 00 32 00 00 09 2e
00 00 09 c2 00 00 00 af  00 00 00 00 00 00 00 00
00 00 00 01 00 00 00 01  00 00 00 32 00 00 00 01
00 00 00 03 00 ff fc 20  00 00 0a 74 00 00 00 14
00 00 00 00 00 00 00 00  00 00 00 04 00 00 00 00
00 00 00 38 00 00 00 08  00 00 00 03 00 ff fc 34
00 00 0a 88 00 00 00 06  00 00 00 00 00 00 00 00
00 00 00 04 00 00 00 00  00 00 00 11 00 00 00 03
00 00 00 00 00 00 00 00  00 00 0a 88 00 00 00 3d
00 00 00 00 00 00 00 00  00 00 00 01 00 00 00 00
00 00 00 01 00 00 00 02  00 00 00 00 00 00 00 00
00 00 0c 30 00 00 07 b0  00 00 00 08 00 00 00 57
00 00 00 04 00 00 00 10  00 00 00 09 00 00 00 03
00 00 00 00 00 00 00 00  00 00 13 e0 00 00 02 a8
00 00 00 00 00 00 00 00  00 00 00 01 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 03 00 00 01
00 00 00 00 00 00 01 00  00 00 00 00 03 00 00 02
00 00 00 00 00 00 09 2e  00 00 00 00 03 00 00 03
00 00 00 00 00 ff fc 20  00 00 00 00 03 00 00 04
00 00 00 00 00 ff fc 34  00 00 00 00 03 00 00 05
00 00 00 01 00 00 00 00  00 00 00 00 04 00 ff f1
00 00 00 0a 00 00 01 0a  00 00 00 00 00 00 00 02
00 00 00 10 00 00 00 00  00 00 00 00 04 00 ff f1
00 00 00 17 00 ff fc 34  00 00 00 04 01 00 00 05
00 00 00 23 00 ff fc 24  00 00 00 04 01 00 00 04
00 00 00 2c 00 00 01 f0  00 00 00 00 00 00 00 02
00 00 00 30 00 00 01 d2  00 00 00 00 00 00 00 02
00 00 00 35 00 00 01 b0  00 00 00 00 00 00 00 02
00 00 00 3a 00 00 01 e6  00 00 00 00 00 00 00 02
00 00 00 3f 00 00 01 62  00 00 00 00 00 00 00 02
00 00 00 44 00 00 02 96  00 00 00 00 00 00 00 02
00 00 00 49 00 00 02 46  00 00 00 00 00 00 00 02
00 00 00 4e 00 00 02 d6  00 00 00 00 00 00 00 02
00 00 00 53 00 00 02 78  00 00 00 00 00 00 00 02
00 00 00 58 00 00 02 8e  00 00 00 00 00 00 00 02
00 00 00 5d 00 00 02 88  00 00 00 00 00 00 00 02
00 00 00 62 00 00 02 48  00 00 00 00 00 00 00 02
00 00 00 67 00 00 02 b6  00 00 00 00 00 00 00 02
00 00 00 6c 00 00 01 66  00 00 00 00 00 00 00 02
00 00 00 71 00 00 02 e0  00 00 00 00 00 00 00 02
00 00 00 76 00 00 00 00  00 00 00 00 04 00 ff f1
00 00 00 7c 00 00 03 04  00 00 00 00 00 00 00 02
00 00 00 80 00 00 03 30  00 00 00 00 00 00 00 02
00 00 00 84 00 00 03 24  00 00 00 00 00 00 00 02
00 00 00 3a 00 00 03 56  00 00 00 00 00 00 00 02
00 00 00 88 00 00 03 58  00 00 00 00 00 00 00 02
00 00 00 8d 00 00 03 8c  00 00 00 00 00 00 00 02
00 00 00 44 00 00 03 7e  00 00 00 00 00 00 00 02
00 00 00 92 00 00 03 ce  00 00 00 00 00 00 00 02
00 00 00 53 00 00 03 f2  00 00 00 00 00 00 00 02
00 00 00 62 00 00 04 22  00 00 00 00 00 00 00 02
00 00 00 58 00 00 04 16  00 00 00 00 00 00 00 02
00 00 00 97 00 00 04 44  00 00 00 00 00 00 00 02
00 00 00 9c 00 00 04 36  00 00 00 00 00 00 00 02
00 00 00 a1 00 00 04 82  00 00 00 00 00 00 00 02
00 00 00 a6 00 00 04 7a  00 00 00 00 00 00 00 02
00 00 00 ab 00 00 04 74  00 00 00 00 00 00 00 02
00 00 00 b0 00 00 04 84  00 00 00 00 00 00 00 02
00 00 00 b5 00 00 04 60  00 00 00 00 00 00 00 02
00 00 00 ba 00 00 04 9e  00 00 00 00 00 00 00 02
00 00 00 bf 00 00 04 96  00 00 00 00 00 00 00 02
00 00 00 c4 00 00 04 bc  00 00 00 00 00 00 00 02
00 00 00 c9 00 00 04 ae  00 00 00 00 00 00 00 02
00 00 00 ce 00 00 04 d2  00 00 00 00 00 00 00 02
00 00 00 d3 00 00 04 ee  00 00 00 00 00 00 00 02
00 00 00 d8 00 00 04 e8  00 00 00 00 00 00 00 02
00 00 00 dd 00 00 04 e2  00 00 00 00 00 00 00 02
00 00 00 e2 00 00 04 f0  00 00 00 00 00 00 00 02
00 00 00 e7 00 00 04 ca  00 00 00 00 00 00 00 02
00 00 00 ec 00 00 05 20  00 00 00 00 00 00 00 02
00 00 00 f1 00 00 05 1a  00 00 00 00 00 00 00 02
00 00 00 f6 00 00 05 14  00 00 00 00 00 00 00 02
00 00 00 fb 00 00 05 2e  00 00 00 00 00 00 00 02
00 00 01 00 00 00 05 28  00 00 00 00 00 00 00 02
00 00 01 05 00 00 05 2c  00 00 00 00 00 00 00 02
00 00 01 0a 00 00 05 06  00 00 00 00 00 00 00 02
00 00 01 0f 00 00 00 00  00 00 00 00 04 00 ff f1
00 00 01 18 00 ff fc 28  00 00 00 0c 01 00 00 04
00 00 01 1e 00 00 05 ba  00 00 00 00 00 00 00 02
00 00 00 84 00 00 06 2e  00 00 00 00 00 00 00 02
00 00 01 22 00 00 00 00  00 00 00 00 04 00 ff f1
00 00 01 1e 00 00 06 b0  00 00 00 00 00 00 00 02
00 00 00 4e 00 00 06 a4  00 00 00 00 00 00 00 02
00 00 00 80 00 00 06 8e  00 00 00 00 00 00 00 02
00 00 00 71 00 00 07 7c  00 00 00 00 00 00 00 02
00 00 00 62 00 00 07 76  00 00 00 00 00 00 00 02
00 00 00 44 00 00 07 6e  00 00 00 00 00 00 00 02
00 00 00 30 00 00 07 60  00 00 00 00 00 00 00 02
00 00 01 2b 00 00 07 06  00 00 00 00 00 00 00 02
00 00 00 35 00 00 07 62  00 00 00 00 00 00 00 02
00 00 00 58 00 00 06 84  00 00 00 00 00 00 00 02
00 00 01 30 00 00 07 8c  00 00 00 00 00 00 00 02
00 00 01 34 00 00 00 00  00 00 00 00 04 00 ff f1
00 00 00 84 00 00 07 f4  00 00 00 00 00 00 00 02
00 00 00 80 00 00 07 fa  00 00 00 00 00 00 00 02
00 00 01 30 00 00 07 fc  00 00 00 00 00 00 00 02
00 00 00 3a 00 00 08 28  00 00 00 00 00 00 00 02
00 00 01 3a 00 00 09 1e  00 00 00 00 00 00 00 02
00 00 00 53 00 00 09 1c  00 00 00 00 00 00 00 02
00 00 00 49 00 00 09 06  00 00 00 00 00 00 00 02
00 00 01 2b 00 00 08 34  00 00 00 00 00 00 00 02
00 00 01 3f 00 ff fc 38  00 00 00 02 11 00 00 05
00 00 01 48 00 00 04 c4  00 00 00 32 10 00 00 02
00 00 01 50 00 00 00 00  00 00 01 00 11 00 00 01
00 00 01 59 00 00 02 ea  00 00 00 2e 10 00 00 02
00 00 01 5f 00 00 09 2e  00 00 00 00 10 00 00 02
00 00 01 66 00 00 03 3c  00 00 00 30 10 00 00 02
00 00 01 6c 00 00 03 18  00 00 00 24 10 00 00 02
00 00 01 72 00 ff fc 20  00 00 00 02 11 00 00 04
00 00 01 79 00 ff fc 34  00 00 00 00 10 00 00 05
00 00 01 84 00 00 01 00  00 00 00 00 10 00 00 02
00 00 01 90 00 00 05 6e  00 00 00 24 10 00 00 02
00 00 01 a7 00 00 06 0c  00 00 00 4a 10 00 00 02
00 00 01 b9 00 00 03 6c  00 00 00 40 10 00 00 02
00 00 01 bf 00 ff fc 3a  00 00 00 00 10 00 00 05
00 00 01 c5 00 00 04 28  00 00 00 28 10 00 00 02
00 00 01 cd 00 00 04 10  00 00 00 18 10 00 00 02
00 00 01 d5 00 00 01 00  00 00 00 00 10 00 00 02
00 00 01 dc 00 ff fc 20  00 00 00 00 10 00 00 04
00 00 01 e8 00 00 08 06  00 00 01 28 10 00 00 02
00 00 01 f2 00 00 05 38  00 00 00 36 10 00 00 02
00 00 01 ff 00 00 03 ac  00 00 00 64 10 00 00 02
00 00 02 08 00 00 04 a6  00 00 00 1e 10 00 00 02
00 00 02 10 00 ff df 20  00 00 00 00 10 00 00 04
00 00 02 1e 00 00 04 50  00 00 00 3e 10 00 00 02
00 00 02 26 00 00 07 a0  00 00 00 66 10 00 00 02
00 00 02 31 00 00 04 f6  00 00 00 42 10 00 00 02
00 00 02 3a 00 00 09 2e  00 00 00 00 10 00 00 03
00 00 02 48 00 ff fc 34  00 00 00 00 10 00 00 04
00 00 02 4f 00 ff fc 3c  00 00 00 00 10 00 ff f1
00 00 02 54 00 00 05 e8  00 00 00 24 10 00 00 02
00 00 02 6b 00 00 04 8e  00 00 00 18 10 00 00 02
00 00 02 73 00 00 06 56  00 00 01 4a 10 00 00 02
00 00 02 80 00 00 05 92  00 00 00 56 10 00 00 02
00 00 02 92 00 ff ff 00  00 00 00 00 10 00 00 05
00 00 02 99 00 00 01 0c  00 00 01 de 10 00 00 02
00 00 02 9f 00 00 09 dd  00 00 00 00 10 00 00 03
00 76 65 63 74 6f 72 2e  63 00 2e 4c 31 02 31 00
6d 61 69 6e 2e 63 00 6c  6f 61 64 62 75 66 5f 5f
5f 30 00 73 69 7a 65 5f  5f 5f 31 00 2e 4c 35 00
2e 4c 33 30 00 2e 4c 33  37 00 2e 4c 31 31 00 2e
4c 34 31 00 2e 4c 31 34  00 2e 4c 31 35 00 2e 4c
34 30 00 2e 4c 32 30 00  2e 4c 33 39 00 2e 4c 32
32 00 2e 4c 33 38 00 2e  4c 32 35 00 2e 4c 33 35
00 2e 4c 32 37 00 6c 69  62 2e 63 00 2e 4c 32 00
2e 4c 38 00 2e 4c 39 00  2e 4c 31 32 00 2e 4c 31
37 00 2e 4c 33 31 00 2e  4c 34 36 00 2e 4c 34 37
00 2e 4c 35 38 00 2e 4c  35 33 00 2e 4c 35 34 00
2e 4c 34 38 00 2e 4c 35  39 00 2e 4c 36 35 00 2e
4c 36 36 00 2e 4c 36 39  00 2e 4c 36 38 00 2e 4c
38 34 00 2e 4c 38 31 00  2e 4c 37 36 00 2e 4c 37
37 00 2e 4c 37 32 00 2e  4c 38 36 00 2e 4c 38 38
00 2e 4c 39 31 00 2e 4c  39 32 00 2e 4c 38 37 00
2e 4c 39 30 00 2e 4c 38  39 00 2e 4c 39 34 00 73
65 72 69 61 6c 2e 63 00  5f 72 65 67 73 00 2e 4c
34 00 78 6d 6f 64 65 6d  2e 63 00 2e 4c 32 31 00
2e 4c 31 00 65 6c 66 2e  63 00 2e 4c 31 30 00 5f
76 61 6c 75 65 5f 31 00  5f 73 74 72 63 6d 70 00
5f 76 65 63 74 6f 72 73  00 5f 70 75 74 63 00 5f
65 74 65 78 74 00 5f 67  65 74 63 00 5f 70 75 74
73 00 5f 76 61 6c 75 65  00 5f 62 73 73 5f 73 74
61 72 74 00 5f 74 65 78  74 5f 73 74 61 72 74 00
5f 73 65 72 69 61 6c 5f  69 73 5f 73 65 6e 64 5f
65 6e 61 62 6c 65 00 5f  73 65 72 69 61 6c 5f 72
65 63 76 5f 62 79 74 65  00 5f 67 65 74 73 00 5f
65 62 73 73 00 5f 6d 65  6d 63 70 79 00 5f 6d 65
6d 73 65 74 00 5f 73 74  61 72 74 00 5f 64 61 74
61 5f 73 74 61 72 74 00  5f 65 6c 66 5f 6c 6f 61
64 00 5f 73 65 72 69 61  6c 5f 69 6e 69 74 00 5f
70 75 74 78 76 61 6c 00  5f 73 74 72 63 70 79 00
5f 62 75 66 66 65 72 5f  73 74 61 72 74 00 5f 6d
65 6d 63 6d 70 00 5f 65  6c 66 5f 63 68 65 63 6b
00 5f 73 74 72 6e 63 6d  70 00 5f 72 6f 64 61 74
61 5f 73 74 61 72 74 00  5f 65 64 61 74 61 00 5f
65 6e 64 00 5f 73 65 72  69 61 6c 5f 69 73 5f 72
65 63 76 5f 65 6e 61 62  6c 65 00 5f 73 74 72 6c
65 6e 00 5f 78 6d 6f 64  65 6d 5f 72 65 63 76 00
5f 73 65 72 69 61 6c 5f  73 65 6e 64 5f 62 79 74
65 00 5f 73 74 61 63 6b  00 5f 6d 61 69 6e 00 5f
65 72 6f 64 61 74 61 00  1a 1a 1a 1a 1a 1a 1a 1a
1a 1a 1a 1a 1a 1a 1a 1a  1a 1a 1a 1a 1a 1a 1a 1a
1a 1a 1a 1a 1a 1a 1a 1a  1a 1a 1a 1a 1a 1a 1a 1a
1a 1a 1a 1a 1a 1a 1a 1a  1a 1a 1a 1a 1a 1a 1a 1a
1a 1a 1a 1a 1a 1a 1a 1a  1a 1a 1a 1a 1a 1a 1a 1a
1a 1a 1a 1a 1a 1a 1a 1a  1a 1a 1a 1a 1a 1a 1a 1a
1a 1a 1a 1a 1a 1a 1a 1a  1a 1a 1a 1a 1a 1a 1a 1a
1a 1a 1a 1a 1a 1a 1a 1a  1a 1a 1a 1a 1a 1a 1a 1a
```

- -> 先頭の `7f 45 4c 46` から、ELF 形式のファイルであることが確認できる。

- 以下の run コマンドで出力されるのはセグメントの領域、つまり Program Header の領域である。

```bash
marinos> run
000094  00000000  00000000  00100  00100  06  01  
000194  00000100  00000100  008dd  008dd  05  01  
000a74  00fffc20  000009dd  00014  0001a  06  01
```

- VAIO 側で転送元の ELF ファイルを readelf で出力された Program Header の結果を以下に示す。

```bash
Program Headers:
  Type           Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align
  LOAD           0x000094 0x00000000 0x00000000 0x00100 0x00100 RW  0x1
  LOAD           0x000194 0x00000100 0x00000100 0x008dd 0x008dd R E 0x1
  LOAD           0x000a74 0x00fffc20 0x000009dd 0x00014 0x0001a RW  0x1
```

- -> run コマンドで出力した結果と readelf コマンドから確認できる結果が一致しているので、実装した elf_load 関数が正常に動作していそうなのが確認できた。
