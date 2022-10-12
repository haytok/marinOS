# 概要

- 11 章で実装した結果とそのプロセスにおいて気になった点を Log に残す。

## 本章で実装すること

- IPC (スレッド間通信) を実装する。

## 実装した結果

```bash
XMODEM receive succeeded :)
boot loader> run
starting from entry point:
ffc020
Boot Succeeded.
Started marinOS ...
Hello World :)
=-=-=-=-=-=-=
=-=-=-=-=-=-=
test11_1 started.
test11_1 recv in.
test11_2 started.
test11_2 send in.
[0] [thread_send]
[1] [thread_send] current->name test11_1
[0] [recvmsg]
[00] [recvmsg] mp->param.p static memory

[000] [recvmsg] mp->param.size f[0000] [recvmsg] mp->sender ffd750
[1] [recvmsg]
[2] [thread_send]
test11_1 recv out.
static memory
test11_1 recv in.
test11_2 send out.
test11_2 send in.
[0] [thread_send]
[1] [thread_send] current->name test11_1
[0] [recvmsg]
[00] [recvmsg] mp->param.p allocated memory

[000] [recvmsg] mp->param.size 12[0000] [recvmsg] mp->sender ffd750
[1] [recvmsg]
[2] [thread_send]
test11_1 recv out.
allocated memory
test11_1 send in.
test11_1 send out.
test11_1 send in.
test11_1 send out.
test11_1 exit.
[thread_end]
test11_1
 [thread_exit] EXIT.
test11_2 send out.
test11_2 recv in.
[0] [recvmsg]
[00] [recvmsg] mp->param.p static memory

[000] [recvmsg] mp->param.size f[0000] [recvmsg] mp->sender ffd718
[1] [recvmsg]
test11_2 recv out.
static memory
test11_2 recv in.
[0] [recvmsg]
[00] [recvmsg] mp->param.p allocated memory

[000] [recvmsg] mp->param.size 12[0000] [recvmsg] mp->sender ffd718
[1] [recvmsg]
test11_2 recv out.
allocated memory
test11_2 exit.
[thread_end]
test11_2
 [thread_exit] EXIT.
```

## 今後の課題

- [ ] 送受信のやりとりをもっと柔軟なように実装したい。

---

- [ ] リセットボタンを押した時にメモリを初期化して再度 OS を起動できるようにできる？？？

## メモ

### p.405 static をつける場合に関して

- 関数の再入が発生させる条件

```c
void log_output(char *msg)
{
  static char buf[256];
  ... (省略)
}
```

- `buf` は `static` な変数なので、他のスレッドから `log_output` が呼び出されるとこの変数が変に書き換えられてしまう場合がある。
- なお、書籍では `buf[]` に `static` を付与しているのはバッファのサイズが 256 Bytes とそれなりに大きいので、スタック上にこの変数分のメモリを確保すると、溢れる場合があるため、システム内に 1 箇所だけ宣言するようにしている。つまり、スレッド毎に確保されるわけではない。
