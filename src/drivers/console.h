#ifndef CONSOLE_H
#define CONSOLE_H

#include "../memory/paging.h"

void put_char(char c);
void put_hex(unsigned long n);
void* paging_map_page(uint64_t phys_addr, uint64_t virt_addr, uint64_t flags);
void console_init(void);

#endif
