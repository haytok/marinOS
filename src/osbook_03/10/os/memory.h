#ifndef _KOZOS_MEMORY_H_INCLUDED_
#define _KOZOS_MEMORY_H_INCLUDED_

int kzmem_init(void);        /* ưŪ����ν���� */
void *kzmem_alloc(int size); /* ưŪ����γ��� */
void kzmem_free(void *mem);  /* ����β��� */

#endif
