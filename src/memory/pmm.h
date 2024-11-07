#ifndef PMM_H
#define PMM_H

#include "../kernel/types.h"

void pmm_init(uint64_t mem_size);
void* pmm_alloc_frame(void);
void pmm_free_frame(void* frame);
uint64_t pmm_get_free_frames(void);

#endif
