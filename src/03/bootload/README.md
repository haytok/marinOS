# 概要

- 3 章で実装した結果とそのプロセスにおいて気になった点を Log に残す。

## 本章で実装すること

- 以下のような main 関数を実装した際に、value の値を書き換えられない。

```c
volatile int value = 10;

int main(void)
{
... (省略)

	// 検証 (変数 value が書き換えられないことを確認する)
	putxval(value, 0);
	PRINT_NEWLINE();
	value = 11;
	putxval(value, 0);

... (省略)
}
```

- 通信した結果、`b` の出力を得られることを期待したが、変数の書き換えが行われず `a` の出力が得られた。

```bash
aytok@haytok-VJS132:~/workspace/marinOS$ kermit                       
Removing stale lock /var/lock/LCK..ttyUSB0 (pid 31373 terminated)      
C-Kermit 9.0.305 OPEN SOURCE: Alpha.07, 24 Jan 2022, for Linux+SSL (64-bit)                                                                   
 Copyright (C) 1985, 2022,                                             
  Trustees of Columbia University in the City of New York.             
Type ? or HELP for help.                                               
(~/workspace/marinOS/) C-Kermit>c                                      
Connecting to /dev/ttyUSB0, speed 9600                                 
 Escape character: Ctrl-\ (ASCII 28, FS): enabled                      
Type the escape character followed by C to get back,                   
or followed by ? to see other options.                                 
----------------------------------------------------                   
Hello World :)                                                         
0a0                                                                    
a                                                                      
a
```

- -> これまで実装したプログラムが ROM に書き込んでいるため、値の変更が不可能だった。そのため、静的変数を ROM ではなく RAM に配置するようにして変数を変更できるようにリンカスクリプトを調整する必要がある。

## 実装した結果

- 検証したプログラム (変数 value が書きかわっていたら良い。)

```c
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
```

- 通信した結果

```bash
haytok@haytok-VJS132:~$ kermit
Removing stale lock /var/lock/LCK..ttyUSB0 (pid 31427 terminated)
C-Kermit 9.0.305 OPEN SOURCE: Alpha.07, 24 Jan 2022, for Linux+SSL (64-bit)
 Copyright (C) 1985, 2022,
  Trustees of Columbia University in the City of New York.
Type ? or HELP for help.
(~/) C-Kermit>c
Connecting to /dev/ttyUSB0, speed 9600
 Escape character: Ctrl-\ (ASCII 28, FS): enabled
Type the escape character followed by C to get back,
or followed by ? to see other options.
----------------------------------------------------
Hello World :)
0a0
a
b
```

- -> 以上の結果より、これまでの実装ではできなかった静的変数の書き換えを行うことができた。

## メモ

- `strip コマンド` により、`kzload.elf` ファイルからデバッグ情報等の不要な情報を削除してサイズを削減できる。

- `readelf -a kozos.elf` を実行した結果を以下に示す。

```bash
[root@cbb840558b6b bootload]# readelf -a kzload.elf
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
  Start of section headers:          2280 (bytes into file)
  Flags:                             0x810000
  Size of this header:               52 (bytes)
  Size of program headers:           32 (bytes)
  Number of program headers:         2
  Size of section headers:           40 (bytes)
  Number of section headers:         8
  Section header string table index: 5

Section Headers:
                                         000000ff (ROM の開始位置)
                                         00007fff (ROM の終了位置)
  [Nr] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al
  [ 0]                   NULL            00000000 000000 000000 00      0   0  0
  [ 1] .vectors          PROGBITS        00000000 000074 000100 00  WA  0   0  4
  [ 2] .text             PROGBITS        00000100 000174 000706 00  AX  0   0  2
  [ 3] .rodata           PROGBITS        00000806 00087a 000023 00   A  0   0  1
  [ 4] .data             PROGBITS        0000082c 0008a0 000010 00  WA  0   0  4
  [ 5] .shstrtab         STRTAB          00000000 0008b0 000038 00      0   0  1
  [ 6] .symtab           SYMTAB          00000000 000a28 000460 10      7  53  4
  [ 7] .strtab           STRTAB          00000000 000e88 000186 00      0   0  1
Key to Flags:
  W (write), A (alloc), X (execute), M (merge), S (strings)
  I (info), L (link order), G (group), T (TLS), E (exclude), x (unknown)
  O (extra OS processing required) o (OS specific), p (processor specific)

There are no section groups in this file.

# セグメントの設定
# リンカスクリプトを修正する前は、VA = PA となっている。
Program Headers:
  Type           Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align
  LOAD           0x000074 0x00000000 0x00000000 0x00829 0x00829 RWE 0x1
  LOAD           0x0008a0 0x0000082c 0x0000082c 0x00010 0x00010 RW  0x1

 Section to Segment mapping:
  Segment Sections...
   00     .vectors .text .rodata
   01     .data

There is no dynamic section in this file.

There are no relocations in this file.

There are no unwind sections in this file.

Symbol table '.symtab' contains 70 entries:
   Num:    Value  Size Type    Bind   Vis      Ndx Name
     0: 00000000     0 NOTYPE  LOCAL  DEFAULT  UND
     1: 00000000     0 SECTION LOCAL  DEFAULT    1
     2: 00000100     0 SECTION LOCAL  DEFAULT    2
     3: 00000806     0 SECTION LOCAL  DEFAULT    3
     4: 0000082c     0 SECTION LOCAL  DEFAULT    4
     5: 00000000     0 FILE    LOCAL  DEFAULT  ABS vector.c
     6: 0000010a     0 NOTYPE  LOCAL  DEFAULT    2 .L1^B1
     7: 00000000     0 FILE    LOCAL  DEFAULT  ABS main.c
     8: 00000806     0 NOTYPE  LOCAL  DEFAULT    3 .LC0
     9: 00000816     0 NOTYPE  LOCAL  DEFAULT    3 .LC1
    10: 0000016e     0 NOTYPE  LOCAL  DEFAULT    2 .L2
    11: 00000000     0 FILE    LOCAL  DEFAULT  ABS lib.c
    12: 0000018e     0 NOTYPE  LOCAL  DEFAULT    2 .L2
    13: 000001d6     0 NOTYPE  LOCAL  DEFAULT    2 .L5
    14: 000001b4     0 NOTYPE  LOCAL  DEFAULT    2 .L4
    15: 0000023e     0 NOTYPE  LOCAL  DEFAULT    2 .L8
    16: 00000252     0 NOTYPE  LOCAL  DEFAULT    2 .L10
    17: 000002a4     0 NOTYPE  LOCAL  DEFAULT    2 .L9
    18: 00000818     0 NOTYPE  LOCAL  DEFAULT    3 .LC0
    19: 00000320     0 NOTYPE  LOCAL  DEFAULT    2 .L14
    20: 000002ea     0 NOTYPE  LOCAL  DEFAULT    2 .L13
    21: 000003b2     0 NOTYPE  LOCAL  DEFAULT    2 .L18
    22: 0000036e     0 NOTYPE  LOCAL  DEFAULT    2 .L17
    23: 000003fc     0 NOTYPE  LOCAL  DEFAULT    2 .L21
    24: 00000496     0 NOTYPE  LOCAL  DEFAULT    2 .L20
    25: 00000490     0 NOTYPE  LOCAL  DEFAULT    2 .L23
    26: 00000464     0 NOTYPE  LOCAL  DEFAULT    2 .L25
    27: 00000452     0 NOTYPE  LOCAL  DEFAULT    2 .L26
    28: 0000045a     0 NOTYPE  LOCAL  DEFAULT    2 .L27
    29: 00000414     0 NOTYPE  LOCAL  DEFAULT    2 .L22
    30: 000004ec     0 NOTYPE  LOCAL  DEFAULT    2 .L30
    31: 000004ca     0 NOTYPE  LOCAL  DEFAULT    2 .L29
    32: 00000540     0 NOTYPE  LOCAL  DEFAULT    2 .L34
    33: 0000055e     0 NOTYPE  LOCAL  DEFAULT    2 .L33
    34: 00000522     0 NOTYPE  LOCAL  DEFAULT    2 .L32
    35: 000005a4     0 NOTYPE  LOCAL  DEFAULT    2 .L39
    36: 00000606     0 NOTYPE  LOCAL  DEFAULT    2 .L38
    37: 000005e8     0 NOTYPE  LOCAL  DEFAULT    2 .L40
    38: 000005d6     0 NOTYPE  LOCAL  DEFAULT    2 .L41
    39: 000005de     0 NOTYPE  LOCAL  DEFAULT    2 .L42
    40: 0000060c     0 NOTYPE  LOCAL  DEFAULT    2 .L36
    41: 0000058a     0 NOTYPE  LOCAL  DEFAULT    2 .L37
    42: 00000654     0 NOTYPE  LOCAL  DEFAULT    2 .L46
    43: 000006cc     0 NOTYPE  LOCAL  DEFAULT    2 .L45
    44: 000006a2     0 NOTYPE  LOCAL  DEFAULT    2 .L47
    45: 00000690     0 NOTYPE  LOCAL  DEFAULT    2 .L48
    46: 00000698     0 NOTYPE  LOCAL  DEFAULT    2 .L49
    47: 000006d2     0 NOTYPE  LOCAL  DEFAULT    2 .L43
    48: 00000638     0 NOTYPE  LOCAL  DEFAULT    2 .L44
    49: 00000000     0 FILE    LOCAL  DEFAULT  ABS serial.c
    50: 00000830    12 OBJECT  LOCAL  DEFAULT    4 _regs
    51: 000007d2     0 NOTYPE  LOCAL  DEFAULT    2 .L5
    52: 000007c2     0 NOTYPE  LOCAL  DEFAULT    2 .L4
    53: 00000572   170 NOTYPE  GLOBAL DEFAULT    2 _strcmp
    54: 00000000   256 OBJECT  GLOBAL DEFAULT    1 _vectors
    55: 00000170    54 NOTYPE  GLOBAL DEFAULT    2 _putc
    56: 000001a6    60 NOTYPE  GLOBAL DEFAULT    2 _puts
    57: 0000082c     2 OBJECT  GLOBAL DEFAULT    4 _value
    58: 00000752    68 NOTYPE  GLOBAL DEFAULT    2 _serial_is_send_enable
    59: 00000334   150 NOTYPE  GLOBAL DEFAULT    2 _memcpy
    60: 000002c2   114 NOTYPE  GLOBAL DEFAULT    2 _memset
    61: 00000100     0 NOTYPE  GLOBAL DEFAULT    2 _start
    62: 000006e2   112 NOTYPE  GLOBAL DEFAULT    2 _serial_init
    63: 000001e2   224 NOTYPE  GLOBAL DEFAULT    2 _putxval
    64: 000004fe   116 NOTYPE  GLOBAL DEFAULT    2 _strcpy
    65: 000003ca   220 NOTYPE  GLOBAL DEFAULT    2 _memcmp
    66: 0000061c   198 NOTYPE  GLOBAL DEFAULT    2 _strncmp
    67: 000004a6    88 NOTYPE  GLOBAL DEFAULT    2 _strlen
    68: 00000796   112 NOTYPE  GLOBAL DEFAULT    2 _serial_send_byte
    69: 0000010c   100 NOTYPE  GLOBAL DEFAULT    2 _main

No version information found in this file.
```

![images/01-1.png](images/01-1.png)

- ? ROM に配置された変数 (ex. regs[] など) を書き込むことができないのは納得できるが、書き込む際、ページフォルト的な感じでエラーみたいなんは発生してるんやろか？？？

- リンカスクリプトの修正に関して

```s
    .rodata : {
        *(.strings)
        *(.rodata)
        *(.rodata.*)
    }

    . = 0xffbf20;
```

- `. = 0xffbf20;` 
- このように、ここにロケーションカウンタを設定しないと、.data と .bss は内臓 ROM の領域に配置される。
- ただ、この設定だけでは正常に動作しない。というか、そももそも h8write が ROM の領域を超えたアドレスの書き込みができない。(保証できない。)

- -> ここで、`ローダー` が役割を発揮する！！！
- -> プログラムの実行時にセグメンテーションの情報をもとに実行ファイルに書き込まれている情報をもとにメモリ上に展開してくれる。

> 変数の初期値は ROM に保存する設定にしておき、h8write でのプログラム転送時にフラッシュ ROM に書き込む。
> 電源 ON でプログラムを起動した際には、プログラムの先頭付近でフラッシュ ROM 上の変数の初期値を RAM 上にコピーする。
> プログラムから変数にアクセスする場合には RAM 上のコピー先のアドレスに対してアクセスする。

- こうすると、ROM から RAM へのコピー処理を行うことで、静的変数が初期値で初期化されていることを保証し、さらに読み書きを可能にする。

> ELF 形式は、セクションの他にセグメントという管理単位を持っています。上述の `readelf コマンド` の出力結果の `Program Headers:` はセグメントの一覧です。プログラムの実行時にはセグメント情報が参照され、メモリ上に展開される。このような作業をロードと呼び、ロードを行うプログラムをローダと呼ぶ。
> 言い換えると、プログラムの実行時には、ローダが実行形式のセグメント情報を読み取り、その通りにメモリ上に展開することになる。ローダーが参照するのはエグメンと情報であり、セクションの情報ではない点には注意しないといけない。
> セクションはリンク時に同じ内容の領域をリンカがまとめるためのものであり、セグメントはプログラムの実行時にローダが参照してメモリ上に展開するためのものである。

- [TIS Committee May 1995 Tool Interface Standard (TIS) Executable and Linking Format (ELF) Specification Version 1.2](https://refspecs.linuxfoundation.org/elf/elf.pdf)

- リンカスクリプトでは、`MEMORY コマンド` を使用することで、メモリ領域を定義し、セクション毎にどの領域に配置されるかを定義することができる。

- ? 仕様書の ROM の終端領域が `0x007ffff` なのか `0x080000` なのかが迷った。引き算してみると、0x1 だったので、アライメントした方の値？の `0x080000` を使うべきなのかと思い、書籍と同様 `romall(rx)` には `l = 0x080000` を設定した。

```bash
haytok@haytok-VJS132:~/workspace/marinOS/src$ python
Python 3.8.13 (default, Aug 23 2022, 09:35:56)
[GCC 10.2.1 20210110] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> 0x080000 - 0x007ffff
1
```

- 仕様書的には `0x000000` ~ `0x0000ff` までがベクタエリアに該当する。この情報から長さは `0x0000ff` となりそうやけど、`l = 0x000100` となっている。イマイチなんでかわからん。ROM の `l` に関しては大きい方を取ってた。

```s
vectors(r) : o = 0x000000, l = 0x000100
```

- ? ROM のサイズは `hex(0x07ffff - 0x0000ff)` で計算していて、小さい方のアドレス (`0x07ffff` or `0x080000`) を取っていた。よくわからん。
- 開始アドレスはちょっと大きい分には問題なさそうやけど、長さに関しては小さくても問題なさそう。

- `> vectors` は MEMORY コマンドで定義したどの領域にセクションを配置するかを指定する。

- `> data` により、data セクションは MEMORY コマンドで定義された data 領域に配置される。
- `AT> rom` は、物理アドレスの指定となる。.data セクションの物理アドレスは rom 領域上に設定される。

- ? 正直、書籍の `AT コマンド` の解説は、イマイチ分かりづらかった。

- 以下の記事が `AT コマンド` の意味を理解するのに参考になった。
  - [GNU C を使いこなそう](https://www.computex.co.jp/article/use_gcc_1.htm)

```
.dataというのは初期値を持ったデータですので、電源を切っても消えないところにその初期値を保持しておく必要があります。組込みシステムでは普通、ROMに保持するのですが、そのときの指定が「AT > ROM」なんです。

そして、その内容をまるごとRAM領域（SYSTEM_RAM）にコピーします。なぜRAMにコピーするかというと、.dataセクションは書き換え可能な変数領域だからです。もし、「int var = 1;」と宣言された変数がROM領域のままだと、「var = 20;」のような書き換えができないことになります。

「変数のアドレス（&var）はコピー先のRAMを指しているの？」と疑問に思われるかもしれませんが、問題ありません。変数のアドレスはRAMを指しつつ、オブジェクトはROM領域に配置するというのが「AT > ROM」の機能です。
```

```
.bssは.dataと違って、コピーするデータを持ちませんので、そのままRAMに配置します。ただし、C言語の仕様で、初期値の無いグローバル変数やstatic変数は0に初期化すべしとありますので、.bssセクションはスタートアップルーチンで0にクリアします。
```

- `_hogehoge` は C 言語のプログラムの中からセクションの位置を知るために重要な役割を果たす。

- p.92 ページの図
  - 内蔵 ROM は 512 KB
  - 内蔵 RAM は 16 KB

- 静的変数の初期値を RAM 上に設定するには、ROM 上の静的変数の初期値を RAM 上にコピーする処理を実装する必要がある。これをやらないと、変数の初期値が正しく設定されないままに、RAM 上の値がそのまま参照されてしまう。なので、追加で init のルーチンを実装する必要がある。
- まぁ、リンカスクリプトから C 言語の変数に何が初期値として設定されてるかとかわからんもんな ...

- リンカスクリプトを変更した後の ELF ファイルの解析結果

```bash
haytok@haytok-VJS132:~/workspace/marinOS/src/03/bootload$ readelf -a kzload.elf
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
  Start of section headers:          2384 (bytes into file)
  Flags:                             0x810000
  Size of this header:               52 (bytes)
  Size of program headers:           32 (bytes)
  Number of program headers:         3
  Size of section headers:           40 (bytes)
  Number of section headers:         8
  Section header string table index: 5

Section Headers:
  [Nr] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al
  [ 0]                   NULL            00000000 000000 000000 00      0   0  0
  [ 1] .vectors          PROGBITS        00000000 000094 000100 00  WA  0   0  4
  [ 2] .text             PROGBITS        00000100 000194 00074e 00  AX  0   0  2
  [ 3] .rodata           PROGBITS        0000084e 0008e2 000023 00   A  0   0  1
  [ 4] .data             PROGBITS        00fffc20 000908 000010 00  WA  0   0  4
  [ 5] .shstrtab         STRTAB          00000000 000918 000038 00      0   0  1
  [ 6] .symtab           SYMTAB          00000000 000a90 000510 10      7  54  4
  [ 7] .strtab           STRTAB          00000000 000fa0 0001ea 00      0   0  1
Key to Flags:
  W (write), A (alloc), X (execute), M (merge), S (strings), I (info),
  L (link order), O (extra OS processing required), G (group), T (TLS),
  C (compressed), x (unknown), o (OS specific), E (exclude),
  D (mbind), p (processor specific)

There are no section groups in this file.

Program Headers:
  Type           Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align
  LOAD           0x000094 0x00000000 0x00000000 0x00100 0x00100 RW  0x1
  LOAD           0x000194 0x00000100 0x00000100 0x00771 0x00771 R E 0x1
  LOAD           0x000908 0x00fffc20 0x00000871 0x00010 0x00010 RW  0x1

 Section to Segment mapping:
  Segment Sections...
   00     .vectors
   01     .text .rodata
   02     .data

There is no dynamic section in this file.

There are no relocations in this file.

The decoding of unwind sections for machine type Renesas H8/300 is not currently supported.

Symbol table '.symtab' contains 81 entries:
   Num:    Value  Size Type    Bind   Vis      Ndx Name
     0: 00000000     0 NOTYPE  LOCAL  DEFAULT  UND
     1: 00000000     0 SECTION LOCAL  DEFAULT    1 .vectors
     2: 00000100     0 SECTION LOCAL  DEFAULT    2 .text
     3: 0000084e     0 SECTION LOCAL  DEFAULT    3 .rodata
     4: 00fffc20     0 SECTION LOCAL  DEFAULT    4 .data
     5: 00000000     0 FILE    LOCAL  DEFAULT  ABS vector.c
     6: 0000010a     0 NOTYPE  LOCAL  DEFAULT    2 .L1^B1
     7: 00000000     0 FILE    LOCAL  DEFAULT  ABS main.c
     8: 0000010c    76 NOTYPE  LOCAL  DEFAULT    2 _init
     9: 0000084e     0 NOTYPE  LOCAL  DEFAULT    3 .LC0
    10: 0000085e     0 NOTYPE  LOCAL  DEFAULT    3 .LC1
    11: 000001b6     0 NOTYPE  LOCAL  DEFAULT    2 .L3
    12: 00000000     0 FILE    LOCAL  DEFAULT  ABS lib.c
    13: 000001d6     0 NOTYPE  LOCAL  DEFAULT    2 .L2
    14: 0000021e     0 NOTYPE  LOCAL  DEFAULT    2 .L5
    15: 000001fc     0 NOTYPE  LOCAL  DEFAULT    2 .L4
    16: 00000286     0 NOTYPE  LOCAL  DEFAULT    2 .L8
    17: 0000029a     0 NOTYPE  LOCAL  DEFAULT    2 .L10
    18: 000002ec     0 NOTYPE  LOCAL  DEFAULT    2 .L9
    19: 00000860     0 NOTYPE  LOCAL  DEFAULT    3 .LC0
    20: 00000368     0 NOTYPE  LOCAL  DEFAULT    2 .L14
    21: 00000332     0 NOTYPE  LOCAL  DEFAULT    2 .L13
    22: 000003fa     0 NOTYPE  LOCAL  DEFAULT    2 .L18
    23: 000003b6     0 NOTYPE  LOCAL  DEFAULT    2 .L17
    24: 00000444     0 NOTYPE  LOCAL  DEFAULT    2 .L21
    25: 000004de     0 NOTYPE  LOCAL  DEFAULT    2 .L20
    26: 000004d8     0 NOTYPE  LOCAL  DEFAULT    2 .L23
    27: 000004ac     0 NOTYPE  LOCAL  DEFAULT    2 .L25
    28: 0000049a     0 NOTYPE  LOCAL  DEFAULT    2 .L26
    29: 000004a2     0 NOTYPE  LOCAL  DEFAULT    2 .L27
    30: 0000045c     0 NOTYPE  LOCAL  DEFAULT    2 .L22
    31: 00000534     0 NOTYPE  LOCAL  DEFAULT    2 .L30
    32: 00000512     0 NOTYPE  LOCAL  DEFAULT    2 .L29
    33: 00000588     0 NOTYPE  LOCAL  DEFAULT    2 .L34
    34: 000005a6     0 NOTYPE  LOCAL  DEFAULT    2 .L33
    35: 0000056a     0 NOTYPE  LOCAL  DEFAULT    2 .L32
    36: 000005ec     0 NOTYPE  LOCAL  DEFAULT    2 .L39
    37: 0000064e     0 NOTYPE  LOCAL  DEFAULT    2 .L38
    38: 00000630     0 NOTYPE  LOCAL  DEFAULT    2 .L40
    39: 0000061e     0 NOTYPE  LOCAL  DEFAULT    2 .L41
    40: 00000626     0 NOTYPE  LOCAL  DEFAULT    2 .L42
    41: 00000654     0 NOTYPE  LOCAL  DEFAULT    2 .L36
    42: 000005d2     0 NOTYPE  LOCAL  DEFAULT    2 .L37
    43: 0000069c     0 NOTYPE  LOCAL  DEFAULT    2 .L46
    44: 00000714     0 NOTYPE  LOCAL  DEFAULT    2 .L45
    45: 000006ea     0 NOTYPE  LOCAL  DEFAULT    2 .L47
    46: 000006d8     0 NOTYPE  LOCAL  DEFAULT    2 .L48
    47: 000006e0     0 NOTYPE  LOCAL  DEFAULT    2 .L49
    48: 0000071a     0 NOTYPE  LOCAL  DEFAULT    2 .L43
    49: 00000680     0 NOTYPE  LOCAL  DEFAULT    2 .L44
    50: 00000000     0 FILE    LOCAL  DEFAULT  ABS serial.c
    51: 00fffc24    12 OBJECT  LOCAL  DEFAULT    4 _regs
    52: 0000081a     0 NOTYPE  LOCAL  DEFAULT    2 .L5
    53: 0000080a     0 NOTYPE  LOCAL  DEFAULT    2 .L4
    54: 000005ba   170 NOTYPE  GLOBAL DEFAULT    2 _strcmp
    55: 00000000   256 OBJECT  GLOBAL DEFAULT    1 _vectors
    56: 000001b8    54 NOTYPE  GLOBAL DEFAULT    2 _putc
    57: 0000084e     0 NOTYPE  GLOBAL DEFAULT    2 _etext
    58: 000001ee    60 NOTYPE  GLOBAL DEFAULT    2 _puts
    59: 00fffc20     2 OBJECT  GLOBAL DEFAULT    4 _value
    60: 00fffc30     0 NOTYPE  GLOBAL DEFAULT    4 _bss_start
    61: 00000100     0 NOTYPE  GLOBAL DEFAULT    2 _text_start
    62: 0000079a    68 NOTYPE  GLOBAL DEFAULT    2 _serial_is_send_[...]
    63: 00fffc30     0 NOTYPE  GLOBAL DEFAULT    4 _ebss
    64: 0000037c   150 NOTYPE  GLOBAL DEFAULT    2 _memcpy
    65: 0000030a   114 NOTYPE  GLOBAL DEFAULT    2 _memset
    66: 00000100     0 NOTYPE  GLOBAL DEFAULT    2 _start
    67: 00fffc20     0 NOTYPE  GLOBAL DEFAULT    4 _data_start
    68: 0000072a   112 NOTYPE  GLOBAL DEFAULT    2 _serial_init
    69: 0000022a   224 NOTYPE  GLOBAL DEFAULT    2 _putxval
    70: 00000546   116 NOTYPE  GLOBAL DEFAULT    2 _strcpy
    71: 00000412   220 NOTYPE  GLOBAL DEFAULT    2 _memcmp
    72: 00000664   198 NOTYPE  GLOBAL DEFAULT    2 _strncmp
    73: 0000084e     0 NOTYPE  GLOBAL DEFAULT    3 _rodata_start
    74: 00fffc30     0 NOTYPE  GLOBAL DEFAULT    4 _edata
    75: 00fffc30     0 NOTYPE  GLOBAL DEFAULT  ABS _end
    76: 000004ee    88 NOTYPE  GLOBAL DEFAULT    2 _strlen
    77: 000007de   112 NOTYPE  GLOBAL DEFAULT    2 _serial_send_byte
    78: 00ffff00     0 NOTYPE  GLOBAL DEFAULT    4 _stack
    79: 00000158    96 NOTYPE  GLOBAL DEFAULT    2 _main
    80: 00000871     0 NOTYPE  GLOBAL DEFAULT    3 _erodata

No version information found in this file.
```

- 初期値なし変数を宣言したときの readelf の出力結果

```bash
haytok@haytok-VJS132:~/workspace/marinOS/src/03/bootload$ readelf -a kzload.elf
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
  Start of section headers:          2392 (bytes into file)
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
  [ 2] .text             PROGBITS        00000100 000194 00074e 00  AX  0   0  2
  [ 3] .rodata           PROGBITS        0000084e 0008e2 000023 00   A  0   0  1
  [ 4] .data             PROGBITS        00fffc20 000908 000010 00  WA  0   0  4
  [ 5] .bss              NOBITS          00fffc30 000918 000002 00  WA  0   0  2
  [ 6] .shstrtab         STRTAB          00000000 000918 00003d 00      0   0  1
  [ 7] .symtab           SYMTAB          00000000 000ac0 000530 10      8  55  4
  [ 8] .strtab           STRTAB          00000000 000ff0 0001f3 00      0   0  1
Key to Flags:
  W (write), A (alloc), X (execute), M (merge), S (strings), I (info),
  L (link order), O (extra OS processing required), G (group), T (TLS),
  C (compressed), x (unknown), o (OS specific), E (exclude),
  D (mbind), p (processor specific)

There are no section groups in this file.

Program Headers:
  Type           Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align
  LOAD           0x000094 0x00000000 0x00000000 0x00100 0x00100 RW  0x1
  LOAD           0x000194 0x00000100 0x00000100 0x00771 0x00771 R E 0x1
  LOAD           0x000908 0x00fffc20 0x00000871 0x00010 0x00012 RW  0x1

 Section to Segment mapping:
  Segment Sections...
   00     .vectors
   01     .text .rodata
   02     .data .bss

There is no dynamic section in this file.

There are no relocations in this file.

The decoding of unwind sections for machine type Renesas H8/300 is not currently supported.

Symbol table '.symtab' contains 83 entries:
   Num:    Value  Size Type    Bind   Vis      Ndx Name
     0: 00000000     0 NOTYPE  LOCAL  DEFAULT  UND
     1: 00000000     0 SECTION LOCAL  DEFAULT    1 .vectors
     2: 00000100     0 SECTION LOCAL  DEFAULT    2 .text
     3: 0000084e     0 SECTION LOCAL  DEFAULT    3 .rodata
     4: 00fffc20     0 SECTION LOCAL  DEFAULT    4 .data
     5: 00fffc30     0 SECTION LOCAL  DEFAULT    5 .bss
     6: 00000000     0 FILE    LOCAL  DEFAULT  ABS vector.c
     7: 0000010a     0 NOTYPE  LOCAL  DEFAULT    2 .L1^B1
     8: 00000000     0 FILE    LOCAL  DEFAULT  ABS main.c
     9: 0000010c    76 NOTYPE  LOCAL  DEFAULT    2 _init
    10: 0000084e     0 NOTYPE  LOCAL  DEFAULT    3 .LC0
    11: 0000085e     0 NOTYPE  LOCAL  DEFAULT    3 .LC1
    12: 000001b6     0 NOTYPE  LOCAL  DEFAULT    2 .L3
    13: 00000000     0 FILE    LOCAL  DEFAULT  ABS lib.c
    14: 000001d6     0 NOTYPE  LOCAL  DEFAULT    2 .L2
    15: 0000021e     0 NOTYPE  LOCAL  DEFAULT    2 .L5
    16: 000001fc     0 NOTYPE  LOCAL  DEFAULT    2 .L4
    17: 00000286     0 NOTYPE  LOCAL  DEFAULT    2 .L8
    18: 0000029a     0 NOTYPE  LOCAL  DEFAULT    2 .L10
    19: 000002ec     0 NOTYPE  LOCAL  DEFAULT    2 .L9
    20: 00000860     0 NOTYPE  LOCAL  DEFAULT    3 .LC0
    21: 00000368     0 NOTYPE  LOCAL  DEFAULT    2 .L14
    22: 00000332     0 NOTYPE  LOCAL  DEFAULT    2 .L13
    23: 000003fa     0 NOTYPE  LOCAL  DEFAULT    2 .L18
    24: 000003b6     0 NOTYPE  LOCAL  DEFAULT    2 .L17
    25: 00000444     0 NOTYPE  LOCAL  DEFAULT    2 .L21
    26: 000004de     0 NOTYPE  LOCAL  DEFAULT    2 .L20
    27: 000004d8     0 NOTYPE  LOCAL  DEFAULT    2 .L23
    28: 000004ac     0 NOTYPE  LOCAL  DEFAULT    2 .L25
    29: 0000049a     0 NOTYPE  LOCAL  DEFAULT    2 .L26
    30: 000004a2     0 NOTYPE  LOCAL  DEFAULT    2 .L27
    31: 0000045c     0 NOTYPE  LOCAL  DEFAULT    2 .L22
    32: 00000534     0 NOTYPE  LOCAL  DEFAULT    2 .L30
    33: 00000512     0 NOTYPE  LOCAL  DEFAULT    2 .L29
    34: 00000588     0 NOTYPE  LOCAL  DEFAULT    2 .L34
    35: 000005a6     0 NOTYPE  LOCAL  DEFAULT    2 .L33
    36: 0000056a     0 NOTYPE  LOCAL  DEFAULT    2 .L32
    37: 000005ec     0 NOTYPE  LOCAL  DEFAULT    2 .L39
    38: 0000064e     0 NOTYPE  LOCAL  DEFAULT    2 .L38
    39: 00000630     0 NOTYPE  LOCAL  DEFAULT    2 .L40
    40: 0000061e     0 NOTYPE  LOCAL  DEFAULT    2 .L41
    41: 00000626     0 NOTYPE  LOCAL  DEFAULT    2 .L42
    42: 00000654     0 NOTYPE  LOCAL  DEFAULT    2 .L36
    43: 000005d2     0 NOTYPE  LOCAL  DEFAULT    2 .L37
    44: 0000069c     0 NOTYPE  LOCAL  DEFAULT    2 .L46
    45: 00000714     0 NOTYPE  LOCAL  DEFAULT    2 .L45
    46: 000006ea     0 NOTYPE  LOCAL  DEFAULT    2 .L47
    47: 000006d8     0 NOTYPE  LOCAL  DEFAULT    2 .L48
    48: 000006e0     0 NOTYPE  LOCAL  DEFAULT    2 .L49
    49: 0000071a     0 NOTYPE  LOCAL  DEFAULT    2 .L43
    50: 00000680     0 NOTYPE  LOCAL  DEFAULT    2 .L44
    51: 00000000     0 FILE    LOCAL  DEFAULT  ABS serial.c
    52: 00fffc24    12 OBJECT  LOCAL  DEFAULT    4 _regs
    53: 0000081a     0 NOTYPE  LOCAL  DEFAULT    2 .L5
    54: 0000080a     0 NOTYPE  LOCAL  DEFAULT    2 .L4
    55: 00fffc30     2 OBJECT  GLOBAL DEFAULT    5 _value_1
    56: 000005ba   170 NOTYPE  GLOBAL DEFAULT    2 _strcmp
    57: 00000000   256 OBJECT  GLOBAL DEFAULT    1 _vectors
    58: 000001b8    54 NOTYPE  GLOBAL DEFAULT    2 _putc
    59: 0000084e     0 NOTYPE  GLOBAL DEFAULT    2 _etext
    60: 000001ee    60 NOTYPE  GLOBAL DEFAULT    2 _puts
    61: 00fffc20     2 OBJECT  GLOBAL DEFAULT    4 _value
    62: 00fffc30     0 NOTYPE  GLOBAL DEFAULT    5 _bss_start
    63: 00000100     0 NOTYPE  GLOBAL DEFAULT    2 _text_start
    64: 0000079a    68 NOTYPE  GLOBAL DEFAULT    2 _serial_is_send_[...]
    65: 00fffc32     0 NOTYPE  GLOBAL DEFAULT    5 _ebss
    66: 0000037c   150 NOTYPE  GLOBAL DEFAULT    2 _memcpy
    67: 0000030a   114 NOTYPE  GLOBAL DEFAULT    2 _memset
    68: 00000100     0 NOTYPE  GLOBAL DEFAULT    2 _start
    69: 00fffc20     0 NOTYPE  GLOBAL DEFAULT    4 _data_start
    70: 0000072a   112 NOTYPE  GLOBAL DEFAULT    2 _serial_init
    71: 0000022a   224 NOTYPE  GLOBAL DEFAULT    2 _putxval
    72: 00000546   116 NOTYPE  GLOBAL DEFAULT    2 _strcpy
    73: 00000412   220 NOTYPE  GLOBAL DEFAULT    2 _memcmp
    74: 00000664   198 NOTYPE  GLOBAL DEFAULT    2 _strncmp
    75: 0000084e     0 NOTYPE  GLOBAL DEFAULT    3 _rodata_start
    76: 00fffc30     0 NOTYPE  GLOBAL DEFAULT    4 _edata
    77: 00fffc34     0 NOTYPE  GLOBAL DEFAULT  ABS _end
    78: 000004ee    88 NOTYPE  GLOBAL DEFAULT    2 _strlen
    79: 000007de   112 NOTYPE  GLOBAL DEFAULT    2 _serial_send_byte
    80: 00ffff00     0 NOTYPE  GLOBAL DEFAULT    5 _stack
    81: 00000158    96 NOTYPE  GLOBAL DEFAULT    2 _main
    82: 00000871     0 NOTYPE  GLOBAL DEFAULT    3 _erodata

No version information found in this file.
```
