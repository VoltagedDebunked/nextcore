#ifndef IDT_H
#define IDT_H

#include "../kernel/types.h"

struct idt_entry
{
    uint16_t base_low;
    uint16_t selector;
    uint8_t always0;
    uint8_t flags;
    uint16_t base_high;
} __attribute__((packed));

struct idt_ptr
{
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

void idt_init(void);
extern void idt_load(uint64_t);
void idt_set_gate(int n, uint64_t base, uint16_t selector, uint8_t flags);

#endif
