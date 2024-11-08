#include "kheap.h"
#include "../kernel/types.h"

#define NULL 0

#define HEAP_START 0x100000    // Start at 1MB
#define HEAP_SIZE  0x400000    // 4MB heap size
#define BLOCK_MAGIC 0x1234DEAD // Magic number to detect heap corruption

typedef struct block_header {
    uint32_t magic;              // Magic number for validation
    size_t size;                 // Size of the block (including header)
    uint8_t is_free;            // 1 if block is free, 0 if allocated
    struct block_header* next;   // Next block in the list
    struct block_header* prev;   // Previous block in the list
} block_header_t;

static block_header_t* heap_start = NULL;
static block_header_t* heap_end = NULL;

// Function to validate a block header
static int is_valid_block(block_header_t* block) {
    return block && block->magic == BLOCK_MAGIC;
}

// Function to create a new block
static block_header_t* create_block(void* addr, size_t size, block_header_t* prev, block_header_t* next) {
    block_header_t* block = (block_header_t*)addr;
    block->magic = BLOCK_MAGIC;
    block->size = size;
    block->is_free = 1;
    block->prev = prev;
    block->next = next;
    return block;
}

void kheap_init(void) {
    // Initialize the first block
    heap_start = (block_header_t*)HEAP_START;
    heap_end = (block_header_t*)(HEAP_START + HEAP_SIZE);

    // Create initial free block
    heap_start = create_block(heap_start, HEAP_SIZE, NULL, NULL);
}

// Function to split a block if it's too large
static void split_block(block_header_t* block, size_t size) {
    size_t remaining_size = block->size - size - sizeof(block_header_t);
    if (remaining_size > sizeof(block_header_t) + 16) { // Minimum block size
        block_header_t* new_block = (block_header_t*)((uint8_t*)block + size + sizeof(block_header_t));
        new_block = create_block(new_block, remaining_size - sizeof(block_header_t),
                               block, block->next);

        if (block->next) {
            block->next->prev = new_block;
        }
        block->next = new_block;
        block->size = size;
    }
}

// Function to merge adjacent free blocks
static void merge_free_blocks(block_header_t* block) {
    if (block->next && block->next->is_free) {
        block->size += block->next->size + sizeof(block_header_t);
        block->next = block->next->next;
        if (block->next) {
            block->next->prev = block;
        }
    }
}

void* kmalloc(size_t size) {
    if (size == 0) return NULL;

    // Align size to 8 bytes
    size = (size + 7) & ~7;

    block_header_t* current = heap_start;

    while (current && current < heap_end) {
        if (!is_valid_block(current)) {
            // Heap corruption detected
            return NULL;
        }

        if (current->is_free && current->size >= size) {
            // Found a suitable block
            split_block(current, size);
            current->is_free = 0;

            // Return pointer to usable memory (after header)
            return (void*)((uint8_t*)current + sizeof(block_header_t));
        }

        current = current->next;
    }

    // No suitable block found
    return NULL;
}

void kfree(void* ptr) {
    if (!ptr) return;

    // Get block header
    block_header_t* block = (block_header_t*)((uint8_t*)ptr - sizeof(block_header_t));

    if (!is_valid_block(block)) {
        // Invalid block or double free
        return;
    }

    block->is_free = 1;

    // Merge with adjacent free blocks
    if (block->prev && block->prev->is_free) {
        block = block->prev;
        merge_free_blocks(block);
    }
    merge_free_blocks(block);
}

// Additional utility functions

size_t kheap_get_used_space(void) {
    size_t used = 0;
    block_header_t* current = heap_start;

    while (current && current < heap_end) {
        if (!current->is_free) {
            used += current->size + sizeof(block_header_t);
        }
        current = current->next;
    }

    return used;
}

size_t kheap_get_free_space(void) {
    size_t free = 0;
    block_header_t* current = heap_start;

    while (current && current < heap_end) {
        if (current->is_free) {
            free += current->size + sizeof(block_header_t);
        }
        current = current->next;
    }

    return free;
}

// Reallocate memory block
void* krealloc(void* ptr, size_t new_size) {
    if (!ptr) return kmalloc(new_size);
    if (new_size == 0) {
        kfree(ptr);
        return NULL;
    }

    block_header_t* block = (block_header_t*)((uint8_t*)ptr - sizeof(block_header_t));
    if (!is_valid_block(block)) return NULL;

    // If the current block is big enough, just return it
    if (block->size >= new_size) {
        split_block(block, new_size);
        return ptr;
    }

    // Allocate new block
    void* new_ptr = kmalloc(new_size);
    if (!new_ptr) return NULL;

    // Copy old data
    for (size_t i = 0; i < block->size; i++) {
        ((uint8_t*)new_ptr)[i] = ((uint8_t*)ptr)[i];
    }

    // Free old block
    kfree(ptr);

    return new_ptr;
}

void* memset(void* dest, int c, size_t n) {
    unsigned char* p = dest;
    while (n--) {
        *p++ = (unsigned char)c;
    }
    return dest;
}
