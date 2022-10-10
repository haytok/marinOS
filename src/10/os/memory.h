#ifndef _MARINOS_MEMORY_H_INCLUDED_
#define _MARINOS_MEMORY_H_INCLUDED_

#include "defines.h"
#include "marinos.h"
#include "lib.h"

int mamem_init(void);
void *mamem_alloc(int size);
void mamem_free(void *mem);

#endif
