# 概要

- 10 章で実装した結果とそのプロセスにおいて気になった点を Log に残す。

## 本章で実装すること

- メモリ管理のロジックを追加実装する。
  - スレッドのアプリケーションから malloc もどきを実行できるようになることがこの章での目標
    - メモリの管理効率を取るか検索の効率性を取るかのトレードオフ

- 作成する関数
  - `mamem_init()`
  - `mamem_alloc()`
  - `mamem_free()`

## 実装した結果

```bash
boot loader> run
starting from entry point:
ffc020
Boot Succeeded.
Started marinOS ...
Hello World :)
=-=-=-=-=-=-=
test10_1 started.
00ffd8a0 aaa
00ffd8b0 bbb
00ffd8b0 aaaaaaa
00ffd8a0 bbbbbbb
00ffd920 aaaaaaaaaaa
00ffd940 bbbbbbbbbbb
00ffd940 aaaaaaaaaaaaaaa
00ffd920 bbbbbbbbbbbbbbb
00ffd920 aaaaaaaaaaaaaaaaaaa
00ffd940 bbbbbbbbbbbbbbbbbbb
00ffd940 aaaaaaaaaaaaaaaaaaaaaaa
00ffd920 bbbbbbbbbbbbbbbbbbbbbbb
00ffda20 aaaaaaaaaaaaaaaaaaaaaaaaaaa
00ffda60 bbbbbbbbbbbbbbbbbbbbbbbbbbb
00ffda60 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
00ffda20 bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
00ffda20 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
00ffda60 bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
00ffda60 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
00ffda20 bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
00ffda20 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
00ffda60 bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
00ffda60 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
00ffda20 bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
00ffda20 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
00ffda60 bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
00ffda60 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
00ffda20 bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
test10_1 exit.
[thread_end]
test10_1
 [thread_exit] EXIT.
```

## 今後の課題

- [ ] K and R 等を参考に malloc() の実装を調査してみる。
  - ヒープ領域の管理方法を調査する。あと、マルチスレッドにおけるフラグメンテーションの回避方法も調査する。
- [ ] メモリの利用効率を上げるためにデータ構造とアルゴリズムを修正する。

## メモ

### p.381 の memory.c のロジックを理解するのに時間を要したのでまとめる

```c
typedef struct _kzmem_block {
	int size;
	struct _kzmem_block *next; // struct をつけるのを忘れない。
} kzmem_block;

typedef struct _kzmem_pool {
	int size;
	int num;
	kzmem_block *free;
} kzmem_pool;

static int kzmem_init_pool(kzmem_pool *p)
{
	// p->free の領域をメモリ用の領域にマッピングし、kzmem_block の配列を作成する。
	int i;
	kzmem_block *mp;
	kzmem_block **mpp;
	static char *area = &freearea;
	mp = (kzmem_block *)area;

	// p->free の値に対するアドレスをメモリ用の領域にマッピングするための変数 mpp の初期化
	mpp = &p->free;
	for (i = 0; i < p->num; i++) {
		// 初回時は p->free の変数のアドレスをメモリ用の領域のアドレスに変更する処理を定義
		// *mpp は mpp の値 (アドレス) が指し示す先の値を示している。
		*mpp = mp;
		// mp の指し示す先の領域の初期化を実施
		memset(mp, 0, sizeof(*mp));
		mp->size = p->size;
		// mp のアドレスを p->size 分進める前に &(mp->next) を保存しておく必要がある。
		mpp = &(mp->next); // mp->next には古い方の mp に対する next のアドレスが入っている。
		mp = (kzmem_block *)((char *)mp + p->size);
		area += p->size;
	}
}
```
