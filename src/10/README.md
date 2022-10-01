# 概要

- 10 章で実装した結果とそのプロセスにおいて気になった点を Log に残す。

## 本章で実装すること

- ...

## 実装した結果

- ...

## 今後の課題

- [ ] ...

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
