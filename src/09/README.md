# 概要

- 9 章で実装した結果とそのプロセスにおいて気になった点を Log に残す。

## 本章で実装すること

- ...

## 実装した結果

- ...

## 今後の課題

- [ ] ...

## メモ

### 

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
