# 概要

- 4 章で実装した結果とそのプロセスにおいて気になった点を Log に残す。

## 本章で実装すること

- ...

## 実装した結果

- ...

## メモ

- ...

> An STX (02) replaces the SOH (01) at the beginning of the transmitted block to notify the receiver of the longer packet length. 

```
ビット 6：レシーブデータレジスタフル（RDRF)

1
RDR に受信データが格納されていることを表示
［セット条件］
シリアル受信が正常終了し、RSR から RDR へ受信データが転送されたとき
```

- -> つまり、1 になっているってことはシリアル受信が正常終了し、RSR レジスタから RDR へ受信データが転送されていることが保証されている。

> fgetc(), getc(), and getchar() return the character read as an unsigned char cast to an int or EOF on end of file or error.

- 今回のライブラリ関数の実装では、int にキャストせずに返す。

- main 関数での処理に関してのメモ

```c
//これは実行できる、つまり buf の変数を puts に引き渡しても問題なく実行できる。
puts(buf);
```

- 以下のレイヤーの下 (いわゆるドライバー) からプログラムを実装していった。その結果、ひとまず、`echo コマンド` もどきは実装できた。なので、これから `XMODEM プロトコル` の実装に移行できる。

+===================+
|       main.c      |
+===================+
|    lib.h lib.c    |
+===================+
| serial.h serial.c |
+===================+
| | | | | | | | | | |
+===================+
|      H8/3069F     |
+===================+

# XMODEM プロトコルの実装と組み込み

- ...
