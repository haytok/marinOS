# 概要

- 8 章で実装した結果とそのプロセスにおいて気になった点を Log に残す。

## 本章で実装すること

- ...

## 実装した結果

- ...

## 今後の課題

- [ ] ...

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
