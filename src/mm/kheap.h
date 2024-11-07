#ifndef KHEAP_H
#define KHEAP_H

#include "../kernel/types.h"

void kheap_init(void);
void* kmalloc(size_t size);
void kfree(void* ptr);
void* krealloc(void* ptr, size_t new_size);
size_t kheap_get_used_space(void);
size_t kheap_get_free_space(void);

#endif
