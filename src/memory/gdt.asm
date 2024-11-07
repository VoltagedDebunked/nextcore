global gdt_flush

gdt_flush:
    lgdt [rdi]    ; Load GDT from the pointer passed in RDI
    mov ax, 0x10  ; Load data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    pop rdi       ; Get return address
    mov rax, 0x08 ; Load code segment selector
    push rax      ; Push code segment
    push rdi      ; Push return address
    retfq         ; Far return to reload CS
.flush:
    ret
