#ifndef ISR_H
#define ISR_H

struct registers {
    unsigned long r15, r14, r13, r12, r11, r10, r9, r8;
    unsigned long rdi, rsi, rbp, rbx, rdx, rcx, rax;
    unsigned long int_no, err_code;
    unsigned long rip, cs, rflags, rsp, ss;
} __attribute__((packed));

void isr_handler(struct registers* regs);

#endif
