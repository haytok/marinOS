# 概要

- 6 章で実装した結果とそのプロセスにおいて気になった点を Log に残す。

## 本章で実装すること

- run すると `send kozos` を実行することでダウンロードされた OS を起動させるための実装
- ELF 形式のファイルのセグメント情報に基づいて RAM の先頭に情報をコピーする。
- エントリーポイントを取得して変更する処理を実装する必要がある。

## 実装した結果

```bash
boot loader> load

(Back at haytok-VJS132)
----------------------------------------------------
(~/workspace/marinOS/src/06/os/) C-Kermit>send marinos
Sending /home/haytok/workspace/marinOS/src/06/os/marinos, 11 blocks: Give your local XMODEM receive command now.
Bytes Sent:   1536   BPS:24

Transfer complete
(~/workspace/marinOS/src/06/os/) C-Kermit>c
Connecting to /dev/ttyUSB0, speed 9600
 Escape character: Ctrl-\ (ASCII 28, FS): enabled
Type the escape character followed by C to get back,
or followed by ? to see other options.
----------------------------------------------------

XMODEM receive succeeded :)
boot loader> run
starting from entry point:
ffc020
Boot Succeeded.
Started marinOS ...
Hello World :)
```

- -> エントリーポイントアドレスは `ffc020` つまり、リンカスクリプトで定義した `ram(rwx)` のアドレスになっていることが確認できた。なので、エントリーポイントは RAM の先頭ギチギチではないこともわかる。(<- p.213 にもあるが、先頭領域を少し空ける理由はイマイチわからん。)

## 今後の課題

- [ ] OS を直接 RAM の戦闘浪域にダウンロードする機能 (p.205)
  - 書籍にはブログを参照とあったが見つからんかった。
    - http://kozos.jp/kozos/
    - https://ameblo.jp/kozos
- [ ] 以下の著者のブログに参考になりそうなものがないかを一通り確認しておく。
    - http://kozos.jp/kozos/
    - https://ameblo.jp/kozos

## メモ

### リンカスクリプト名に関して

- リンカスクリプトのハイライトをつけるために VSCode の拡張機能を追加した。この拡張機能では `scr` の拡張子をサポートしてなかったので、拡張子を `ld` に変更した。後で不具合があったら元に戻そう。

### リンカスクリプトで定義する RAM のアドレスに関して

```c
    ramall(rwx) : o = 0xffbf20, l = 0x004000
    /* readelf marinos から確認できるエントリーポイントアドレスとなる。 */
    /* ram(rwx)    : o = 0xffbf20, l = 0x003f00 */
    ram(rwx)    : o = 0xffc020, l = 0x003f00
```

- 書籍的には `ram` の先頭アドレスを `0xffc020` にしているが、手元で検証したところ `0xffbf20` でも問題ないことを確認した。いかにその結果を示す。

```bash
boot loader> load

(Back at haytok-VJS132)
----------------------------------------------------
(~/workspace/marinOS/src/06/os/) C-Kermit>send marinos
Sending /home/haytok/workspace/marinOS/src/06/os/marinos, 11 blocks: Give your local XMODEM receive command now.
Bytes Sent:   1536   BPS:24

Transfer complete
(~/workspace/marinOS/src/06/os/) C-Kermit>c
Connecting to /dev/ttyUSB0, speed 9600
 Escape character: Ctrl-\ (ASCII 28, FS): enabled
Type the escape character followed by C to get back,
or followed by ? to see other options.
----------------------------------------------------

XMODEM receive succeeded :)
boot loader> run
starting from entry point:
ffbf20
Boot Succeeded.
Started marinOS ...
Hello World :)
marinos> echo hello
 hello
```
