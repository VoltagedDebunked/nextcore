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
OBJECTS += $(SRC_DIR)/memory/gdt_asm.o $(SRC_DIR)/interrupt/idt_asm.o

all: kernel.bin bzImage
	@echo
	@echo "NextCore Compiled Successfully!"
	@echo

kernel.bin: $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

bzImage: kernel.bin
	# Create a bzImage from the kernel ELF binary
	# First, create a flat binary
	objcopy -O binary kernel.bin kernel.raw
	# Then, compress it using gzip and prepend the required header for bzImage
	# Note: Adjust the size and other parameters as necessary
	# The `--pad-to` option is used to specify the total size of the bzImage
	# This is often set to 512 KiB or 1 MiB
	dd if=/dev/zero bs=1 count=512 of=bzImage.img
	cat kernel.raw | gzip -9 > kernel.gz
	cat kernel.gz >> bzImage.img
	# Add bzImage header
	dd if=bzImage.img of=bzImage bs=512 conv=notrunc

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(SRC_DIR)/memory/gdt_asm.o: $(SRC_DIR)/memory/gdt.asm
	$(AS) $(ASFLAGS) $< -o $@

$(SRC_DIR)/interrupt/idt_asm.o: $(SRC_DIR)/interrupt/idt.asm
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -f $(SRC_DIR)/*/*/*.o kernel.bin bzImage kernel.raw kernel.gz $(SRC_DIR)/*/*.o $(SRC_DIR)/*.o
