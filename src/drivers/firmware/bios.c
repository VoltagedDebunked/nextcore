#include "bios.h"
#include "../io.h"

// BIOS interrupt numbers
#define BIOS_DISK_INTERRUPT    0x13
#define BIOS_MEMORY_INTERRUPT  0x15
#define BIOS_VIDEO_INTERRUPT   0x10

// BIOS services function numbers
#define BIOS_DISK_RESET       0x00
#define BIOS_DISK_READ        0x02
#define BIOS_DISK_PARAMS      0x08
#define BIOS_GET_MEMORY_MAP   0xE820
#define BIOS_GET_MEMORY_SIZE  0xE801

// Global variables
static int is_initialized = 0;

void bios_init(void) {
    if (is_initialized) return;

    // Basic initialization if needed
    is_initialized = 1;
}

int bios_get_memory_map(bios_memory_map_entry_t* entries, int max_entries) {
    uint32_t continuation = 0;
    int count = 0;

    while (count < max_entries) {
        bios_memory_map_entry_t* entry = &entries[count];

        // Use inline assembly to call BIOS interrupt
        uint32_t signature = 0x534D4150;  // 'SMAP'
        uint32_t bytes;

        __asm__ volatile (
            "int $0x15"
            : "=a" (signature),
              "=b" (continuation),
              "=c" (bytes)
            : "a" (0xE820),
              "b" (continuation),
              "c" (24),
              "d" (signature),
              "D" (entry)
            : "memory"
        );

        if (signature != 0x534D4150) break;

        count++;
        if (continuation == 0) break;
    }

    return count;
}

uint32_t bios_get_memory_size(void) {
    uint32_t extended_kb;

    __asm__ volatile (
        "int $0x15"
        : "=a" (extended_kb)
        : "a" (0xE801)
        : "ebx", "ecx", "edx"
    );

    return extended_kb * 1024;  // Convert KB to bytes
}

void bios_disk_reset(uint8_t drive) {
    __asm__ volatile (
        "int $0x13"
        :
        : "a" (0x0000),
          "d" (drive)
    );
}

int bios_disk_read(uint8_t drive, uint16_t cylinder, uint16_t head,
                   uint16_t sector, uint8_t count, void* buffer) {
    uint8_t status;

    __asm__ volatile (
        "int $0x13"
        : "=a" (status)
        : "a" ((0x02 << 8) | count),
          "b" (buffer),
          "c" (((cylinder & 0xFF) << 8) | sector),
          "d" ((head << 8) | drive)
        : "memory"
    );

    return (status >> 8) == 0;  // Return 1 on success, 0 on failure
}

int bios_get_disk_parameters(uint8_t drive, uint8_t* drive_type,
                           uint16_t* cylinders, uint16_t* sectors,
                           uint16_t* heads) {
    uint8_t status;
    uint16_t cx, dx;

    __asm__ volatile (
        "int $0x13"
        : "=a" (status),
          "=c" (cx),
          "=d" (dx)
        : "a" (0x0800),
          "d" (drive)
        : "bx"
    );

    if ((status >> 8) != 0) return 0;

    if (drive_type) *drive_type = (status & 0xFF);
    if (cylinders) *cylinders = ((cx & 0xC0) << 2) | ((cx & 0xFF00) >> 8);
    if (sectors) *sectors = (cx & 0x3F);
    if (heads) *heads = ((dx & 0xFF00) >> 8) + 1;

    return 1;
}

void bios_reboot(void) {
    // Perform a warm reboot using keyboard controller
    outb(0x64, 0xFE);
}

void bios_shutdown(void) {
    // Try APM shutdown (might not work on modern systems, fuck it though)
    __asm__ volatile (
        "mov $0x5301, %ax\n"
        "xor %bx, %bx\n"
        "int $0x15\n"

        "mov $0x530E, %ax\n"
        "xor %bx, %bx\n"
        "mov $0x0102, %cx\n"
        "int $0x15\n"

        "mov $0x5307, %ax\n"
        "mov $0x0001, %bx\n"
        "mov $0x0003, %cx\n"
        "int $0x15\n"
    );

    // If APM shutdown fails, halt the CPU
    for (;;) {
        __asm__ volatile ("hlt");
    }
}
