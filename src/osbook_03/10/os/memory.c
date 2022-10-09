#include "defines.h"
#include "kozos.h"
#include "lib.h"
#include "memory.h"

/*
 * ���ꡦ�֥��å���¤��
 * (�������줿���ΰ�ϡ���Ƭ�˰ʲ��ι�¤�Τ���äƤ���)
 */
typedef struct _kzmem_block {
	struct _kzmem_block *next;
	int size;
} kzmem_block;

/* ���ꡦ�ס��� */
typedef struct _kzmem_pool {
	int size;
	int num;
	kzmem_block *free;
} kzmem_pool;

/* ���ꡦ�ס�������(�ġ��Υ������ȸĿ�) */
static kzmem_pool pool[] = {
	{ 16, 8, NULL },
	{ 32, 8, NULL },
	{ 64, 4, NULL },
};

#define MEMORY_AREA_NUM (sizeof(pool) / sizeof(*pool))

/* ���ꡦ�ס���ν���� */
static int kzmem_init_pool(kzmem_pool *p)
{
	int i;
	kzmem_block *mp;
	kzmem_block **mpp;
	extern char freearea;
	static char *area = &freearea;

	mp = (kzmem_block *)area;

	puts("mp ");
	putxval(mp, 0);
	puts("\n");

	puts("&p->free->next ");
	putxval(&p->free->next, 0);
	puts("\n");

	mpp = &p->free;

	puts("mpp ");
	putxval(mpp, 0);
	puts("\n");

	puts("[-1] &p->free->next ");
	putxval(&p->free->next, 0);
	puts(", mp ");
	putxval(mp, 0);
	puts(", mp->next ");
	putxval(mp->next, 0);
	puts("\n");

	puts("~~~~~~~~~");
	puts("\n");

	for (i = 0; i < p->num; i++) {
		*mpp = mp;

		puts("[0] &p->free->next ");
		putxval(&p->free->next, 0);
		puts(", mp ");
		putxval(mp, 0);
		puts(", mp->next ");
		putxval(mp->next, 0);
		puts("\n");

		memset(mp, 0, sizeof(*mp));
		mp->size = p->size;

		// puts("mp->next ");
		// putxval(mp->next, 0); // 0
		// puts("\n");
		// puts("&mp->next ");
		// putxval(&(mp->next), 0); // ffd2a8
		// puts("\n");
		// puts("-----\n");

		mpp = &(mp->next);

		puts("[1] &p->free->next ");
		putxval(&p->free->next, 0);
		puts(", mp ");
		putxval(mp, 0);
		puts(", mp->next ");
		putxval(mp->next, 0);
		puts("\n");

		// puts("mpp ");
		// putxval(mpp, 0); // ffd2a8
		// puts("-----\n");
		// puts("*mpp ");
		// putxval(*mpp, 0); // 0
		// puts("-+-+-+-+-\n");

		mp = (kzmem_block *)((char *)mp + p->size);
		area += p->size;

		puts("[2] &p->free->next ");
		putxval(&p->free->next, 0);
		puts(", mp ");
		putxval(mp, 0);
		puts(", mp->next ");
		putxval(mp->next, 0);
		puts("\n");
	}

	puts("&p->free->next ");
	putxval(&p->free->next, 0);
	puts(", mp ");
	putxval(mp, 0);
	puts(", mp->next ");
	putxval(mp->next, 0);
	puts("\n");

	return 0;
}

/* ưŪ����ν���� */
int kzmem_init(void)
{
	int i;
	for (i = 0; i < MEMORY_AREA_NUM; i++) {
		kzmem_init_pool(&pool[i]); /* �ƥ��ꡦ�ס������������ */
	}
	return 0;
}

/* ưŪ����γ��� */
void *kzmem_alloc(int size)
{
	int i;
	kzmem_block *mp;
	kzmem_pool *p;

	for (i = 0; i < MEMORY_AREA_NUM; i++) {
		p = &pool[i];
		if (size <= p->size - sizeof(kzmem_block)) {
			if (p->free == NULL) { /* �����Ѥ��ΰ褬̵��(���ꡦ�֥��å���­) */
				kz_sysdown();
				return NULL;
			}
			/* �����Ѥߥ�󥯥ꥹ�Ȥ����ΰ��������� */
			mp = p->free;
			p->free = p->free->next;
			mp->next = NULL;

			/*
       * �ºݤ����Ѳ�ǽ���ΰ�ϡ����ꡦ�֥��å���¤�Τ�ľ����ΰ��
       * �ʤ�Τǡ�ľ��Υ��ɥ쥹���֤���
       */
			return mp + 1;
		}
	}

	/* ���ꤵ�줿���������ΰ���Ǽ�Ǥ�����ꡦ�ס��뤬̵�� */
	kz_sysdown();
	return NULL;
}

/* ����β��� */
void kzmem_free(void *mem)
{
	int i;
	kzmem_block *mp;
	kzmem_pool *p;

	/* �ΰ��ľ���ˤ���(�Ϥ���)���ꡦ�֥��å���¤�Τ���� */
	mp = ((kzmem_block *)mem - 1);

	for (i = 0; i < MEMORY_AREA_NUM; i++) {
		p = &pool[i];
		if (mp->size == p->size) {
			/* �ΰ������Ѥߥ�󥯥ꥹ�Ȥ��᤹ */
			mp->next = p->free;
			p->free = mp;
			return;
		}
	}

	kz_sysdown();
}
