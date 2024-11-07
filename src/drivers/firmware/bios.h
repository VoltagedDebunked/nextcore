#ifndef BIOS_H
#define BIOS_H

#include "../../kernel/types.h"

// BIOS Parameter Block structure
typedef struct {
    uint16_t bytes_per_sector;
    uint8_t  sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t  number_of_fats;
    uint16_t root_dir_entries;
    uint16_t total_sectors;
    uint8_t  media_descriptor;
    uint16_t sectors_per_fat;
    uint16_t sectors_per_track;
    uint16_t heads_per_cylinder;
    uint32_t hidden_sectors;
    uint32_t large_sector_count;
} __attribute__((packed)) bios_parameter_block_t;

// BIOS Memory Map Entry structure
typedef struct {
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;
    uint32_t acpi;
} __attribute__((packed)) bios_memory_map_entry_t;

// Function declarations
void bios_init(void);
int bios_get_memory_map(bios_memory_map_entry_t* entries, int max_entries);
uint32_t bios_get_memory_size(void);
void bios_disk_reset(uint8_t drive);
int bios_disk_read(uint8_t drive, uint16_t cylinder, uint16_t head,
                   uint16_t sector, uint8_t count, void* buffer);
int bios_get_disk_parameters(uint8_t drive, uint8_t* drive_type,
                           uint16_t* cylinders, uint16_t* sectors,
                           uint16_t* heads);
void bios_reboot(void);
void bios_shutdown(void);

#endif
