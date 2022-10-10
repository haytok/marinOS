# 概要

- 8 章で実装した結果とそのプロセスにおいて気になった点を Log に残す。

## 本章で実装すること

- スレッドを作成する。

## 実装した結果

```bash
(~/workspace/marinOS/src/08/os/) C-Kermit>c
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
[ma_start] &current->context : ffccd8
[start_threads]
[thread_intr] [0] sp : fff4a4start
[thread_intr] [1] sp : fff4a4
[start_threads] END
[thread_intr] [0] sp : fff4c0start
start[thread_exit] EXIT.
[thread_intr] [1] sp : fff4c0
[test08_1_main] Started.
>echo hello
 hello
>e
unknown.
>echo haytok
 haytok
>exit
[test08_1_main] Exit.
[thread_intr] [0] sp : fff5c0command
command[thread_exit] EXIT.
[ma_sysdown] System Error.
```

## 今後の課題

- [ ] バックスペースを実装したい。
- [ ] printf もどきの関数を実装したい。
  - いちいち puts 関数で頑張るのがめんどくさいので ...
- [ ] エラーハンドリングを十分に実装する。
  - 例えば ma_syscall 関数のエラーハンドリング (-1 が返ってきた時など) が不足している。
- [ ] 確認できる範囲でのスタックポインタを追ってみる。
- [ ] 処理のフローをシーケンス図にまとめたい。
- [ ] 確認したスタックポインタを元に攻撃 or スタックの破壊ができないかを検証してみる。
  - 破壊するためのプログラムの実装のイメージを以下に示す。以下のプログラムを RAM に書き込み、exit をしまくるとスタックが破壊されて、bootloader に処理が戻る。原因はわからんかった ... :(

```c
static int start_threads(int argc, char *argv[])
{
	puts("[start_threads]\n");

	ma_run(test08_1_main, "command", 0x100, 0, NULL);
	ma_run(test08_1_main, "test_2", 0x100, 0, NULL);
	ma_run(test08_1_main, "test_3", 0x100, 0, NULL);
	ma_run(test08_1_main, "test_4", 0x100, 0, NULL);
	ma_run(test08_1_main, "test_5", 0x100, 0, NULL);
	ma_run(test08_1_main, "test_6", 0x100, 0, NULL);
	ma_run(test08_1_main, "test_7", 0x100, 0, NULL);
	ma_run(test08_1_main, "test_8", 0x100, 0, NULL);
	ma_run(test08_1_main, "test_9", 0x100, 0, NULL);
	ma_run(test08_1_main, "test_10", 0x100, 0, NULL);

	puts("[start_threads] END\n");

	return 0;
}
```

## メモ

### thread_run　関数内の変数 thread_stack の値に関して

- 以下のように `thread_stack` の値が変化するかどうかを確認するための検証を行う。

```c
static kz_thread_id_t thread_run(kz_func_t func, char *name, int priority,
				 int stacksize, int argc, char *argv[])
{
	int i;
	kz_thread *thp;
	uint32 *sp;
	extern char userstack;
	static char *thread_stack = &userstack;

	puts("hoge\n");
	putxval(thread_stack, 0);
	puts("hoge\n");

... (省略)

  memset(thread_stack, 0, stacksize);
  thread_stack += stacksize;

  thp->stack = thread_stack;

... (省略)
}
```

- 結果

```bash
kzload> run
starting from entry point: ffc020
kozos boot succeed!
hoge
fff400hoge
hoge
fff500hoge
hoge
fff600hoge
hoge
fff700hoge
test09_1 started.
test09_1 sleep in.
test09_2 started.
test09_2 sleep in.
test09_3 started.
test09_3 wakeup in (test09_1).
test09_1 sleep out.
test09_1 chpri in.
test09_3 wakeup out.
test09_3 wakeup in (test09_2).
test09_2 sleep out.
test09_2 chpri in.
test09_1 chpri out.
test09_1 wait in.
test09_3 wakeup out.
test09_3 wait in.
test09_2 chpri out.
test09_2 wait in.
test09_1 wait out.
test09_1 trap in.
test09_1 DOWN.
test09_1 EXIT.
test09_3 wait out.
test09_3 exit in.
test09_3 EXIT.
test09_2 wait out.
test09_2 exit.
test09_2 EXIT.
Killed
```

- なお、以下の C 言語のプログラムでも検証してみた。

```c
#include <stdio.h>

static char userstack = 0x40;

void test()
{
	printf("userstack %x\n", userstack);
	static char *thread_stack = &userstack;
	printf("thread_stack %x\n", thread_stack);
	int stacksize = 0x10;
	thread_stack += stacksize;
}

int main(void)
{
	test();
	puts("=====");
	test();
}
```

- 結果

```bash
haytok@haytok-VJS132:~/workspace/marinOS$ gcc -o main main.c && ./main
userstack 40
thread_stack 55a03010
=====
userstack 40
thread_stack 55a03020
```

- -> 確かに `stacksize` の分だけ `thread_stack` の値が増えているのが確認できた。なお、`thread_stack` に `static` をつけないと、増えることはなかった。

- `static 修飾子`に関して

> ローカル変数は通常、関数呼び出しとともに生成され、returnされることで破棄されます。しかし、static付きのローカル変数は破棄されなくなります。

- [C言語 staticを変数と関数に付ける価値【保護の仕組みを解説】](https://monozukuri-c.com/langc-funclist-static/)

#### まとめ

- `thread_run 関数` 内の`変数 thread_stack` の値は `thread_run 関数` が呼び出されるたびにインクリメントされていき、別の領域のスタックが確保される。(これがわかったのはデカい。)

## start_threads 関数に 3 つのスレッドを起動させるようにしたときの挙動を確認してみる

- main.c の start_threads 関数内の処理を以下のように変更して、build して RAM に転送する。

```c
static int start_threads(int argc, char *argv[])
{
	kz_run(test08_1_main, "command", 0x100, 0, NULL);
	kz_run(test08_2_main, "command", 0x100, 0, NULL);
	kz_run(test08_3_main, "command", 0x100, 0, NULL);
	return 0;
}
```

- 実行結果

```bash
(~/workspace/marinOS/src/osbook_03/08/os/) C-Kermit>c
Connecting to /dev/ttyUSB0, speed 9600
 Escape character: Ctrl-\ (ASCII 28, FS): enabled
Type the escape character followed by C to get back,
or followed by ? to see other options.
----------------------------------------------------

XMODEM receive succeeded.
kzload> echo hello
unknown.
kzload> run
starting from entry point: ffc020
kozos boot succeed!
test08_1 started.
> echo hello
 hello
> exit
test08_1 exit.
command EXIT.
test08_2 started.
> echo haytok
 haytok
> echo haytok
 haytok
> exit
test08_2 exit.
command EXIT.
start EXIT.
test08_3 started.
> eixi
unknown.
> exit
test08_3 exit.
command EXIT.
system error!
```

- -> 各スレッドが exit して kz_run の処理を抜けてから、kz_run の処理が再開しているのが確認できた。

### スタックに使用方法に関して

```bash
boot loader> run
starting from entry point:
ffc020
Boot Succeeded.
Started marinOS ...
Hello World :)
marinos> �marinos boot loader started ... :)
boot loader> cho 
unknown.
```

### メモ

- 2022/09/26 はこの章のデバッグ (OS が起動しない) 原因の調査から開始する。
  - -> defines.h の uint_32 の定義を間違えて int にしていのが悪かった ... print デバッグで dispatch 移行の処理がおかしいことに気づけたのはエラい〜〜〜 (dispatch の直前までは処理が走っていたのは確認できていた。)

```c
typedef unsigned long uint_32;
```

### デバッグの際に使用した Log

```bash
boot loader> run
starting from entry point:
ffc020
Boot Succeeded.
Started marinOS ...
Hello World :)
[thread_run] tnp: ffd198
[thread_run]fff4dc
[ma_start] current->name : start
[thread_init] [0]
[start_threads] START
[thread_intr] [0] current->name : start
[thread_intr] [00] sp : fff49c
[thread_intr] [00] &current->context : ffd1c4
[thread_intr] [000] &current->context : ffd1c4
[thread_intr] [000] current->context.sp : fff49c
[thread_intr] [000] &current->context.sp : ffd1c4
[thread_run] tnp: ffd198
[thread_run] tnp: ffd1c8
[thread_run]fff5dc
[0000] [thread_intr] [after handlers[type]] &current->context : ffd1f4
[00000] [thread_intr] [before schedule()] current->name : command1
[1] [thread_intr] [after schedule()] current->name : start
[2] [thread_intr] [after schedule()] &current->context : ffd1c4
=-=-=-=-=-=-=
[thread_intr] [0] current->name : start
[thread_intr] [00] sp : fff494
[thread_intr] [00] &current->context : ffd1c4
[thread_intr] [000] &current->context : ffd1c4
[thread_intr] [000] current->context.sp : fff494
[thread_intr] [000] &current->context.sp : ffd1c4
[thread_run] tnp: ffd198
[thread_run] tnp: ffd1c8
[thread_run] tnp: ffd1f8
[thread_run]fff6dc
[0000] [thread_intr] [after handlers[type]] &current->context : ffd224
[00000] [thread_intr] [before schedule()] current->name : command2
[1] [thread_intr] [after schedule()] current->name : command1
[2] [thread_intr] [after schedule()] &current->context : ffd1f4
[thread_init] [0]
[test08_1_main] Started.
>exit
[test08_1_main] Exit.
[thread_init] [1]
[thread_end]
[thread_intr] [0] current->name : command1
[thread_intr] [00] sp : fff5bc
[thread_intr] [00] &current->context : ffd1f4
[thread_intr] [000] &current->context : ffd1f4
[thread_intr] [000] current->context.sp : fff5bc
[thread_intr] [000] &current->context.sp : ffd1f4
command1
 [thread_exit] EXIT.
[0000] [thread_intr] [after handlers[type]] &current->context : ffd1f4
[00000] [thread_intr] [before schedule()] current->name :
[1] [thread_intr] [after schedule()] current->name : start
[2] [thread_intr] [after schedule()] &current->context : ffd1c4
=-=-=-=-=-=-=
[start_threads] END
[thread_init] [1]
[thread_end]
[thread_intr] [0] current->name : start
[thread_intr] [00] sp : fff4bc
[thread_intr] [00] &current->context : ffd1c4
[thread_intr] [000] &current->context : ffd1c4
[thread_intr] [000] current->context.sp : fff4bc
[thread_intr] [000] &current->context.sp : ffd1c4
start
 [thread_exit] EXIT.
[0000] [thread_intr] [after handlers[type]] &current->context : ffd1c4
[00000] [thread_intr] [before schedule()] current->name :
[1] [thread_intr] [after schedule()] current->name : command2
[2] [thread_intr] [after schedule()] &current->context : ffd224
[thread_init] [0]
[test08_2_main] Started.
>echo hello
 hello
>dump
unknown.
>exit
[test08_2_main] Exit.
[thread_init] [1]
[thread_end]
[thread_intr] [0] current->name : command2
[thread_intr] [00] sp : fff6bc
[thread_intr] [00] &current->context : ffd224
[thread_intr] [000] &current->context : ffd224
[thread_intr] [000] current->context.sp : fff6bc
[thread_intr] [000] &current->context.sp : ffd224
command2
 [thread_exit] EXIT.
[0000] [thread_intr] [after handlers[type]] &current->context : ffd224
[00000] [thread_intr] [before schedule()] current->name :
[ma_sysdown] System Error.
```
