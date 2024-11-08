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

    while (1)
    {
        // Kernel main loop
        __asm__ volatile("hlt");
    }
}
