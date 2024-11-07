#include "irq.h"

void irq_handler(void) {
    // Send EOI to slave PIC first (IRQ8-15)
    outb(0x20, 0xA0);
    // Send EOI to master PIC (IRQ0-7)
    outb(0x20, 0x20);
}
