#include "memory.h"

// memory block 用の構造体を定義
typedef struct _mamem_block {
	struct _mamem_block *next;
	int size;
} mamem_block;

// memory pool 用の構造体を定義
typedef struct _mamem_pool {
	int size;
	int num;
	mamem_block *free;
} mamem_pool;

static mamem_pool pool[] = {
	{ 16, 8, NULL },
	{ 32, 8, NULL },
	{ 64, 4, NULL },
};

#define MEMORY_AREA_NUM (sizeof(pool) / sizeof(*pool))

// p->free を初期化
static int mamem_init_pool(mamem_pool *p)
{
	int i;
	extern char freearea;
	static char *area = &freearea;
	mamem_block *mp;
	mamem_block **mpp;

	mp = (mamem_block *)area;

	// p->free を area に紐づける必要がある。
	mpp = &p->free;
	for (i = 0; i < p->num; i++) {
		*mpp = mp;
		memset(mp, 0, sizeof(*mp));
		mp->size = p->size;
		// 今セットアップしている構造体のオブジェクトの next のフィールドのアドレスを一時保存している。
		// この値はメモリのアドレスを進めて次のアドレスが明らかになった時に、この取得したアドレスを介して next に次のオブジェクトのアドレスを設定する。
		mpp = &(mp->next);
		mp = ((mamem_block *)((char *)mp + p->size));
		area += p->size;
	}

	return 0;
}

int mamem_init(void)
{
	int i;

	for (i = 0; i < MEMORY_AREA_NUM; i++) {
		mamem_init_pool(&pool[i]);
	}

	return 0;
}

// メモリ管理ライブラリ (システムコール内から呼び出される。)
void *mamem_alloc(int size)
{
	int i;
	mamem_pool *p;
	mamem_block *mp;

	for (i = 0; i < MEMORY_AREA_NUM; i++) {
		p = &pool[i];
		if (size <= p->size - sizeof(mamem_block)) {
			if (p->free == NULL) {
				ma_sysdown();
				return NULL;
			}
			mp = p->free;
			p->free = p->free->next;
			mp->next = NULL;

			// ヘッダ部分のアドレスをインクリメントさせる。
			return mp + 1;
		}
	}

	ma_sysdown();

	return NULL;
}

void mamem_free(void *mem)
{
	int i;
	mamem_block *mp;
	mamem_pool *p;

	mp = ((mamem_block *)mem - 1);

	for (i = 0; i < MEMORY_AREA_NUM; i++) {
		p = &pool[i];
		if (p->size == mp->size) {
			// tail を使ってないから先頭に無理やり突っ込むしかない ...
			mp->next = p->free;
			p->free = mp;
			return;
		}
	}

	ma_sysdown();
}
