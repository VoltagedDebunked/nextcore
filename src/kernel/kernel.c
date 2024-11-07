#include "types.h"
#include "../memory/gdt.h"
#include "../interrupt/idt.h"
#include "../memory/paging.h"
#include "../drivers/keyboard.h"
#include "../drivers/timer.h"
#include "../mm/kheap.h"

void kernel_main(void)
{
    gdt_init();
    idt_init();
    init_paging();
    keyboard_init();
    timer_init();
    kheap_init();

    while (1)
    {
        // Kernel main loop
        __asm__ volatile("hlt");
    }
}
