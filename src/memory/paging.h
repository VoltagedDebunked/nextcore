#ifndef PAGING_H
#define PAGING_H

#include "../kernel/types.h"

void init_paging(void);
void switch_page_table(uint64_t* page_table);
uint64_t* get_page(uint64_t address, int make, uint64_t* dir);

#endif
