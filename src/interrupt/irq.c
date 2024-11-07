#include "irq.h"

void irq_handler(void)
{
    unsigned char dummy;

    // Send EOI to slave PIC first (IRQ8-15)
    __asm__ volatile("outb %%al, $0xA0" : : "a"(0x20));

    // Send EOI to master PIC (IRQ0-7)
    __asm__ volatile("outb %%al, $0x20" : : "a"(0x20));

    // Read from master PIC to ensure interrupt was acknowledged
    __asm__ volatile("inb $0x20, %%al" : "=a"(dummy));

    // Read from slave PIC to ensure interrupt was acknowledged
    __asm__ volatile("inb $0xA0, %%al" : "=a"(dummy));

    (void)dummy; // To avoid unused variable warning, i hate those so much...
}
