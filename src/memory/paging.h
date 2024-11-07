#ifndef PAGING_H
#define PAGING_H

#include "../kernel/types.h"

#define PAGE_SIZE 4096
#define KERNEL_VIRTUAL_BASE 0xFFFFFFFF80000000

void init_paging(void);
void* paging_map_page(uint64_t phys_addr, uint64_t virt_addr, uint64_t flags);
void paging_unmap_page(uint64_t virt_addr);
uint64_t paging_get_phys_addr(uint64_t virt_addr);

#endif
