global _start
extern kernel_main

section .text
bits 64

_start:
    ; Set up the stack
    mov rsp, stack_top

    ; Clear direction flag
    cld

    ; Clear registers
    xor rax, rax
    xor rbx, rbx
    xor rcx, rcx
    xor rdx, rdx
    xor rsi, rsi
    xor rdi, rdi
    xor rbp, rbp
    xor r8, r8
    xor r9, r9
    xor r10, r10
    xor r11, r11
    xor r12, r12
    xor r13, r13
    xor r14, r14
    xor r15, r15

    ; Jump to kernel
    call kernel_main

    ; If we return from kernel_main, halt the CPU
.halt:
    cli             ; Disable interrupts
    hlt             ; Halt the CPU
    jmp .halt       ; Just in case an NMI happens

section .bss
align 16
stack_bottom:
    resb 16384     ; 16 KB stack
stack_top:
