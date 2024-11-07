global idt_load

idt_load:
    lidt [rdi]    ; Load IDT from the pointer passed in RDI. Fucking hell i have to use assembly
    ret
