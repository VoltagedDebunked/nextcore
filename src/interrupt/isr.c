#include "isr.h"
#include "../drivers/console.h"

static void print(const char* str) {
    while(*str) {
        put_char(*str++);
    }
}

void isr_handler(struct registers* regs) {
    switch(regs->int_no) {
        case 0:
            print("Division Error at RIP: ");
            put_hex(regs->rip);
            for(;;); // Halt
            break;

        case 14: {
            unsigned long cr2;
            asm volatile ("mov %%cr2, %0" : "=r"(cr2));
            print("Page Fault at: ");
            put_hex(cr2);
            print(" RIP: ");
            put_hex(regs->rip);
            for(;;); // Halt
            break;
        }

        default:
            print("Interrupt: ");
            put_hex(regs->int_no);
            break;
    }
}
