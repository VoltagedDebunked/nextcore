CC = gcc
AS = nasm
CFLAGS = -m64 -mcmodel=large -ffreestanding -fno-stack-protector -fno-pie -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -nostdlib -I./src/kernel -I./src/memory -I./src/interrupt -I./src/drivers -I./src/mm
LDFLAGS = -T scripts/linker.ld -nostdlib -no-pie -Wl,--build-id=none
ASFLAGS = -f elf64

SRC_DIR = src
KERNEL_SOURCES = $(wildcard $(SRC_DIR)/kernel/*.c)
MEMORY_SOURCES = $(wildcard $(SRC_DIR)/memory/*.c)
INTERRUPT_SOURCES = $(wildcard $(SRC_DIR)/interrupt/*.c)
DRIVER_SOURCES = $(wildcard $(SRC_DIR)/drivers/*.c)
MM_SOURCES = $(wildcard $(SRC_DIR)/mm/*.c)

SOURCES = $(KERNEL_SOURCES) $(MEMORY_SOURCES) $(INTERRUPT_SOURCES) $(DRIVER_SOURCES) $(MM_SOURCES)
OBJECTS = $(SOURCES:.c=.o)
OBJECTS += $(SRC_DIR)/boot/boot.o $(SRC_DIR)/memory/gdt_asm.o $(SRC_DIR)/interrupt/idt_asm.o

all: kernel.bin
	@echo
	@echo "NextCore Compiled Successfully!"
	@echo

kernel.bin: $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(SRC_DIR)/boot/boot.o: $(SRC_DIR)/boot/boot.asm
	$(AS) $(ASFLAGS) $< -o $@

$(SRC_DIR)/memory/gdt_asm.o: $(SRC_DIR)/memory/gdt.asm
	$(AS) $(ASFLAGS) $< -o $@

$(SRC_DIR)/interrupt/idt_asm.o: $(SRC_DIR)/interrupt/idt.asm
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -f $(SRC_DIR)/*/*/*.o kernel.bin $(SRC_DIR)/*/*.o $(SRC_DIR)/*.o
