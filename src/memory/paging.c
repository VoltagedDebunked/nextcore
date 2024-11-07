#include "paging.h"

uint64_t next_free_page;

uint64_t next_free_page = 0;

uint64_t* pml4;

void init_paging(void)
{
    pml4 = (uint64_t*)0x1000;
    for (int i = 0; i < 512; i++)
    {
        pml4[i] = 0;
    }

    uint64_t* pdp = (uint64_t*)0x2000;
    pml4[0] = (uint64_t)pdp | 3;

    for (int i = 0; i < 512; i++)
     {
        pdp[i] = 0;
    }

    uint64_t* pd = (uint64_t*)0x3000;
    pdp[0] = (uint64_t)pd | 3;

    for (int i = 0; i < 512; i++)
    {
        pd[i] = 0;
    }

    uint64_t* pt = (uint64_t*)0x4000;
    pd[0] = (uint64_t)pt | 3;

    for (int i = 0; i < 512; i++)
    {
        pt[i] = 0;
    }
}

void switch_page_table(uint64_t* page_table) {
    // Load the physical address of the page table into CR3 register
    __asm__ volatile("mov %0, %%cr3" : : "r" (page_table));
}

extern uint64_t next_free_page;


void* allocate_page(void) {
    void* page = (void*)next_free_page;
    next_free_page += 4096; // 4KB page
    return page;
}

uint64_t* get_page(uint64_t address, int make, uint64_t* dir) {
    // Convert virtual address to indices
    uint64_t pd_index = (address >> 39) & 0x1FF;   // Get PML4 index
    uint64_t pdp_index = (address >> 30) & 0x1FF;  // Get PDPT index
    uint64_t pt_index = (address >> 21) & 0x1FF;   // Get PD index
    uint64_t p_index = (address >> 12) & 0x1FF;    // Get PT index

    // Navigate page table hierarchy
    uint64_t* pdp_table;
    uint64_t* pd_table;
    uint64_t* pt_table;

    // Check if PML4 entry exists
    if (!(dir[pd_index] & 0x1)) {
        if (!make) {
            return 0;
        }
        // Create new PDPT
        pdp_table = (uint64_t*)allocate_page();
        if (!pdp_table) {
            return 0;
        }
        // Clear the new table
        for (int i = 0; i < 512; i++) {
            pdp_table[i] = 0;
        }
        // Set the PML4 entry (present + writable + user)
        dir[pd_index] = ((uint64_t)pdp_table) | 0x7;
    } else {
        pdp_table = (uint64_t*)(dir[pd_index] & ~0xFFF);
    }

    // Check if PDPT entry exists
    if (!(pdp_table[pdp_index] & 0x1)) {
        if (!make) {
            return 0;
        }
        // Create new PD
        pd_table = (uint64_t*)allocate_page();
        if (!pd_table) {
            return 0;
        }
        // Clear the new table
        for (int i = 0; i < 512; i++) {
            pd_table[i] = 0;
        }
        // Set the PDPT entry (present + writable + user)
        pdp_table[pdp_index] = ((uint64_t)pd_table) | 0x7;
    } else {
        pd_table = (uint64_t*)(pdp_table[pdp_index] & ~0xFFF);
    }

    // Check if PD entry exists
    if (!(pd_table[pt_index] & 0x1)) {
        if (!make) {
            return 0;
        }
        // Create new PT
        pt_table = (uint64_t*)allocate_page();
        if (!pt_table) {
            return 0;
        }
        // Clear the new table
        for (int i = 0; i < 512; i++) {
            pt_table[i] = 0;
        }
        // Set the PD entry (present + writable + user)
        pd_table[pt_index] = ((uint64_t)pt_table) | 0x7;
    } else {
        pt_table = (uint64_t*)(pd_table[pt_index] & ~0xFFF);
    }

    // Return pointer to the page table entry
    return &pt_table[p_index];
}
