# 概要

- 2 章で実装した結果とそのプロセスにおいて気になった点を Log に残す。

## 本章で実装すること

- ライブラリ関数と 16 進数を出力する関数の追加
  - `memset() 関数`
  - `memcpy() 関数`
  - `memcmp() 関数`
  - `strlen() 関数`
  - `srtcpy() 関数`
  - `strncmp() 関数`
  - `memset() 関数`
  - `putxval() 関数`
- シリアル通信で

## 実装した結果

- Currently being implemented ... 

<!-- ```bash
haytok@haytok-VJS132:~$ kermit
Removing stale lock /var/lock/LCK..ttyUSB0 (pid 212941 terminated)
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
Hello World :)Hell�Hello World :)Hello World :)Hello World :)Killed
``` -->

<!-- - -> 以上の結果より、想定した挙動であることが確認できた :) -->

## メモ

- シリアル通信によるデータ転送の制御を CPU がソフトウェア的に行うのは現実的に厳しい。そのため、シリアル通信の制御を行うための IC チップを一般的にコントローラと呼ぶ。通信の信号の制御はコントローラが行うため、CPU からはコントローラに対してデータの送信要求を出したり、コントローラからデータを受信したりするだけで済むようになる。
- なお、シリアル・コントローラなら、コントローラの持つ特定のレジスタに 1 byte を書き込むと、あとはコントローラ側でその 1 byte をシリアル送信する。

- H8/3069F マイコン・ボードのシリアルコネクタには SCI が接続されているため、SCI1 を操作することでシリアル経由での入出力を行える。 (書籍 p.64)

![images/02-1.png](images/02-1.png)
