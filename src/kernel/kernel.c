#include "types.h"
#include "../memory/gdt.h"
#include "../interrupt/idt.h"
#include "../memory/paging.h"
#include "../drivers/keyboard.h"
#include "../drivers/timer.h"
#include "../drivers/console.h"
#include "../mm/kheap.h"
#include "../fs/ext4.h"
#include "../fs/vfs/vfs.h"

// Simulated CPU memory request structure
typedef struct {
    void* address;
    size_t size;
    bool is_active;
} MemoryRequest;

#define MAX_MEMORY_REQUESTS 10
#define MAX_MEMORY_SIZE 1024  // Max allocation size

// Global memory request tracking
static MemoryRequest memory_requests[MAX_MEMORY_REQUESTS];
static volatile uint32_t request_counter = 0;

// Memory management simulation functions
void init_memory_requests() {
    for (int i = 0; i < MAX_MEMORY_REQUESTS; i++) {
        memory_requests[i].is_active = false;
    }
}

MemoryRequest* create_memory_request(size_t size) {
    // Find an empty slot
    for (int i = 0; i < MAX_MEMORY_REQUESTS; i++) {
        if (!memory_requests[i].is_active) {
            // Allocate memory
            void* mem = kmalloc(size);

            if (mem) {
                memory_requests[i].address = mem;
                memory_requests[i].size = size;
                memory_requests[i].is_active = true;

                // Print allocation details
                put_char('A');  // Allocation marker
                put_hex((unsigned long)mem);
                put_char(' ');
                put_hex(size);
                put_char('\n');

                return &memory_requests[i];
            }
            break;
        }
    }
    return NULL;
}

void free_memory_request(MemoryRequest* request) {
    if (request && request->is_active) {
        // Print free details
        put_char('F');  // Free marker
        put_hex((unsigned long)request->address);
        put_char(' ');
        put_hex(request->size);
        put_char('\n');

        // Free the memory
        kfree(request->address);
        request->is_active = false;
    }
}

// Simulated memory stress test
void memory_stress_test() {
    // Periodically create and free memory
    static uint32_t stress_cycle = 0;

    switch (stress_cycle % 4) {
        case 0: {
            // Allocate small memory block
            size_t size = (request_counter % MAX_MEMORY_SIZE) + 16;
            create_memory_request(size);
            break;
        }

        case 1: {
            // Reallocate existing memory
            for (int i = 0; i < MAX_MEMORY_REQUESTS; i++) {
                if (memory_requests[i].is_active) {
                    size_t new_size = memory_requests[i].size * 2;
                    if (new_size <= MAX_MEMORY_SIZE) {
                        void* new_mem = krealloc(memory_requests[i].address, new_size);
                        if (new_mem) {
                            memory_requests[i].address = new_mem;
                            memory_requests[i].size = new_size;

                            put_char('R');  // Reallocation marker
                            put_hex((unsigned long)new_mem);
                            put_char(' ');
                            put_hex(new_size);
                            put_char('\n');
                        }
                    }
                    break;
                }
            }
            break;
        }

        case 2: {
            // Free a random active memory request
            for (int i = 0; i < MAX_MEMORY_REQUESTS; i++) {
                if (memory_requests[i].is_active) {
                    free_memory_request(&memory_requests[i]);
                    break;
                }
            }
            break;
        }

        case 3: {
            // Memory usage report
            size_t used = kheap_get_used_space();
            size_t free = kheap_get_free_space();

            put_char('M');  // Memory report marker
            put_char('U');
            put_hex(used);
            put_char('F');
            put_hex(free);
            put_char('\n');
            break;
        }
    }

    stress_cycle++;
    request_counter++;
}

void kernel_main(void)
{
    gdt_init();
    idt_init();
    init_paging();
    ext4_init();
    console_init();
    vfs_init();
    keyboard_init();
    timer_init();
    kheap_init();

    // Initialize memory request tracking
    init_memory_requests();

    while (1)
    {
        // Perform memory management operations
        memory_stress_test();

        // Optional: Add some delay or condition to control memory operations
        for (volatile int delay = 0; delay < 100000; delay++) {
            // Small delay to prevent overwhelming the system
        }

        // Halt the CPU to save power
        __asm__ volatile("hlt");
    }
}
