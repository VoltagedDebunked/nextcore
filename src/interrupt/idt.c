#include "idt.h"

struct idt_entry idt[256];
struct idt_ptr idtp;

extern void idt_load(uint64_t);

void idt_set_gate(int n, uint64_t base, uint16_t selector, uint8_t flags)
{
    idt[n].base_low = base & 0xFFFF;
    idt[n].base_high = (base >> 16) & 0xFFFF;
    idt[n].selector = selector;
    idt[n].always0 = 0;
    idt[n].flags = flags;
}

void idt_init(void)
{
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base = (uint64_t)&idt;

    for (int i = 0; i < 256; i++)
    {
        idt_set_gate(i, 0, 0, 0);
    }

    idt_load((uint64_t)&idtp);
}
