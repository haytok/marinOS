#include "defines.h"
#include "kozos.h"
#include "lib.h"
#include "memory.h"

/*
 * ���ꡦ�֥�å���¤��
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
  { 16, 8, NULL }, { 32, 8, NULL }, { 64, 4, NULL },
};

#define MEMORY_AREA_NUM (sizeof(pool) / sizeof(*pool))

/* ���ꡦ�ס���ν���� */
static int kzmem_init_pool(kzmem_pool *p)
{
  int i;
  kzmem_block *mp;
  kzmem_block **mpp;
  extern char freearea; /* ��󥫡�������ץȤ�������������ΰ� */
  static char *area = &freearea;

  mp = (kzmem_block *)area;

  /* �ġ����ΰ�򤹤٤Ʋ����Ѥߥ�󥯥ꥹ�Ȥ˷Ҥ� */
  mpp = &p->free;
  for (i = 0; i < p->num; i++) {
    *mpp = mp;
    memset(mp, 0, sizeof(*mp));
    mp->size = p->size;
    mpp = &(mp->next);
    mp = (kzmem_block *)((char *)mp + p->size);
    area += p->size;
  }

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
      if (p->free == NULL) { /* �����Ѥ��ΰ褬̵��(���ꡦ�֥�å���­) */
	kz_sysdown();
	return NULL;
      }
      /* �����Ѥߥ�󥯥ꥹ�Ȥ����ΰ��������� */
      mp = p->free;
      p->free = p->free->next;
      mp->next = NULL;

      /*
       * �ºݤ����Ѳ�ǽ���ΰ�ϡ����ꡦ�֥�å���¤�Τ�ľ����ΰ��
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

  /* �ΰ��ľ���ˤ���(�Ϥ���)���ꡦ�֥�å���¤�Τ���� */
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
