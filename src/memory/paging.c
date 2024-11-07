#include "paging.h"
#include "pmm.h"

#define NULL 0

#define PAGE_PRESENT (1ULL << 0)
#define PAGE_WRITABLE (1ULL << 1)
#define PAGE_USER (1ULL << 2)

typedef struct {
    uint64_t entries[512];
} page_table_t;

static page_table_t* kernel_p4;

static page_table_t* get_next_level(page_table_t* table, uint64_t index, bool create) {
    if (!(table->entries[index] & PAGE_PRESENT) && create) {
        void* frame = pmm_alloc_frame();
        if (!frame) return NULL;

        table->entries[index] = (uint64_t)frame | PAGE_PRESENT | PAGE_WRITABLE;
        page_table_t* next_table = (page_table_t*)((uint64_t)frame + KERNEL_VIRTUAL_BASE);
        for (int i = 0; i < 512; i++) {
            next_table->entries[i] = 0;
        }
        return next_table;
    }
    return (page_table_t*)((table->entries[index] & ~0xFFF) + KERNEL_VIRTUAL_BASE);
}

void init_paging(void) {
    // Allocate P4 table
    kernel_p4 = (page_table_t*)pmm_alloc_frame();

    // Clear P4
    for (int i = 0; i < 512; i++) {
        kernel_p4->entries[i] = 0;
    }

    // Identity map first 2MB
    for (uint64_t addr = 0; addr < 0x200000; addr += PAGE_SIZE) {
        paging_map_page(addr, addr, PAGE_PRESENT | PAGE_WRITABLE);
    }

    // Map kernel to higher half
    for (uint64_t addr = 0; addr < 0x1000000; addr += PAGE_SIZE) {
        paging_map_page(addr, addr + KERNEL_VIRTUAL_BASE, PAGE_PRESENT | PAGE_WRITABLE);
    }

    // Load P4 into CR3
    asm volatile("mov %0, %%cr3" : : "r"(kernel_p4));
}

void* paging_map_page(uint64_t phys_addr, uint64_t virt_addr, uint64_t flags) {
    uint64_t p4_index = (virt_addr >> 39) & 0x1FF;
    uint64_t p3_index = (virt_addr >> 30) & 0x1FF;
    uint64_t p2_index = (virt_addr >> 21) & 0x1FF;
    uint64_t p1_index = (virt_addr >> 12) & 0x1FF;

    page_table_t* p4 = kernel_p4;
    page_table_t* p3 = get_next_level(p4, p4_index, true);
    page_table_t* p2 = get_next_level(p3, p3_index, true);
    page_table_t* p1 = get_next_level(p2, p2_index, true);

    if (!p1) return NULL;

    p1->entries[p1_index] = phys_addr | flags;
    return (void*)virt_addr;
}
