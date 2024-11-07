#include "ext4.h"
#include "../mm/kheap.h"
#include "../kernel/types.h"
#include "../drivers/io.h"

#define SECTOR_SIZE 512
#define SUPERBLOCK_OFFSET 1024
#define NULL 0

// Internal structures and definitions
typedef struct {
    uint32_t block_size;
    uint32_t groups_count;
    uint32_t blocks_per_group;
    uint32_t inodes_per_group;
    struct ext4_superblock* sb;
} ext4_fs_ctx;

// Global filesystem context
static ext4_fs_ctx fs_ctx = {0};

// Utility functions for disk I/O
static void wait_disk() {
    while (inb(0x1F7) & 0x80);
}

static void read_disk_sector(uint32_t lba, uint8_t* buffer) {
    wait_disk();

    outb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
    outb(0x1F2, 1);
    outb(0x1F3, lba & 0xFF);
    outb(0x1F4, (lba >> 8) & 0xFF);
    outb(0x1F5, (lba >> 16) & 0xFF);
    outb(0x1F7, 0x20);

    wait_disk();

    for (int i = 0; i < 256; i++) {
        uint16_t data = inb(0x1F0) | (inb(0x1F0) << 8);
        buffer[i*2] = data & 0xFF;
        buffer[i*2+1] = (data >> 8) & 0xFF;
    }
}

static void write_disk_sector(uint32_t lba, const uint8_t* buffer) {
    wait_disk();

    outb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
    outb(0x1F2, 1);
    outb(0x1F3, lba & 0xFF);
    outb(0x1F4, (lba >> 8) & 0xFF);
    outb(0x1F5, (lba >> 16) & 0xFF);
    outb(0x1F7, 0x30);

    wait_disk();

    for (int i = 0; i < 256; i++) {
        uint16_t data = buffer[i*2] | (buffer[i*2+1] << 8);
        outb(0x1F0, data & 0xFF);
        outb(0x1F0, (data >> 8) & 0xFF);
    }

    wait_disk();
}

static uint8_t* read_block(uint32_t block_num) {
    uint8_t* buffer = kmalloc(fs_ctx.block_size);
    if (!buffer) return NULL;

    uint32_t start_sector = block_num * (fs_ctx.block_size / SECTOR_SIZE);
    uint32_t sector_count = fs_ctx.block_size / SECTOR_SIZE;

    for (uint32_t i = 0; i < sector_count; i++) {
        read_disk_sector(start_sector + i, buffer + (i * SECTOR_SIZE));
    }

    return buffer;
}

static int write_block(uint32_t block_num, const uint8_t* data) {
    uint32_t start_sector = block_num * (fs_ctx.block_size / SECTOR_SIZE);
    uint32_t sector_count = fs_ctx.block_size / SECTOR_SIZE;

    for (uint32_t i = 0; i < sector_count; i++) {
        write_disk_sector(start_sector + i, data + (i * SECTOR_SIZE));
    }

    return 0;
}

// Group descriptor operations
static struct ext4_group_desc* read_group_desc(uint32_t group_num) {
    uint32_t block_group_descriptor = fs_ctx.block_size == 1024 ? 2 : 1;
    uint32_t offset = group_num * sizeof(struct ext4_group_desc);

    uint8_t* block = read_block(block_group_descriptor);
    if (!block) return NULL;

    struct ext4_group_desc* desc = kmalloc(sizeof(struct ext4_group_desc));
    if (!desc) {
        kfree(block);
        return NULL;
    }

    // Copy group descriptor data
    for (uint32_t i = 0; i < sizeof(struct ext4_group_desc); i++) {
        ((uint8_t*)desc)[i] = block[offset + i];
    }

    kfree(block);
    return desc;
}

// Implementation of public functions
int ext4_init(void) {
    // Allocate and initialize superblock
    fs_ctx.sb = kmalloc(sizeof(struct ext4_superblock));
    if (!fs_ctx.sb) return -1;

    // Read superblock
    if (ext4_read_superblock(fs_ctx.sb) != 0) {
        kfree(fs_ctx.sb);
        return -1;
    }

    // Verify filesystem
    if (fs_ctx.sb->s_magic != EXT4_SUPER_MAGIC) {
        kfree(fs_ctx.sb);
        return -1;
    }

    // Initialize filesystem context
    fs_ctx.block_size = 1024 << fs_ctx.sb->s_log_block_size;
    fs_ctx.blocks_per_group = fs_ctx.sb->s_blocks_per_group;
    fs_ctx.inodes_per_group = fs_ctx.sb->s_inodes_per_group;
    fs_ctx.groups_count = (fs_ctx.sb->s_blocks_count_lo + fs_ctx.blocks_per_group - 1)
                         / fs_ctx.blocks_per_group;

    return 0;
}

int ext4_read_superblock(struct ext4_superblock* sb) {
    uint8_t buffer[SECTOR_SIZE * 2];
    read_disk_sector(2, buffer);
    read_disk_sector(3, buffer + SECTOR_SIZE);

    // Copy superblock data
    for (uint32_t i = 0; i < sizeof(struct ext4_superblock); i++) {
        ((uint8_t*)sb)[i] = buffer[i + (SUPERBLOCK_OFFSET - SECTOR_SIZE)];
    }

    return 0;
}

int ext4_write_superblock(const struct ext4_superblock* sb) {
    uint8_t buffer[SECTOR_SIZE * 2];
    read_disk_sector(2, buffer);
    read_disk_sector(3, buffer + SECTOR_SIZE);

    // Update superblock data
    for (uint32_t i = 0; i < sizeof(struct ext4_superblock); i++) {
        buffer[i + (SUPERBLOCK_OFFSET - SECTOR_SIZE)] = ((uint8_t*)sb)[i];
    }

    write_disk_sector(2, buffer);
    write_disk_sector(3, buffer + SECTOR_SIZE);
    return 0;
}

int ext4_read_inode(uint32_t inode_num, struct ext4_inode* inode) {
    if (!inode || inode_num < EXT4_ROOT_INO) return -1;

    // Calculate the block and offset for the inode
    uint32_t block_group = (inode_num - 1) / fs_ctx.inodes_per_group;
    uint32_t index = (inode_num - 1) % fs_ctx.inodes_per_group;
    uint32_t inode_table_block = read_group_desc(block_group)->bg_inode_table;

    uint32_t inode_size = sizeof(struct ext4_inode);
    uint32_t block_offset = index * inode_size;

    // Read the inode block
    uint8_t* block = read_block(inode_table_block + block_offset / fs_ctx.block_size);
    if (!block) return -1;

    // Copy inode data
    for (uint32_t i = 0; i < inode_size; i++) {
        ((uint8_t*)inode)[i] = block[block_offset + i];
    }

    kfree(block);
    return 0;
}

int ext4_write_inode(uint32_t inode_num, const struct ext4_inode* inode) {
    if (inode_num < EXT4_ROOT_INO) return -1;

    // Calculate the block and offset for the inode
    uint32_t block_group = (inode_num - 1) / fs_ctx.inodes_per_group;
    uint32_t index = (inode_num - 1) % fs_ctx.inodes_per_group;
    uint32_t inode_table_block = read_group_desc(block_group)->bg_inode_table;

    uint32_t inode_size = sizeof(struct ext4_inode);
    uint32_t block_offset = index * inode_size;

    // Read the inode block
    uint8_t* block = read_block(inode_table_block + block_offset / fs_ctx.block_size);
    if (!block) return -1;

    // Update inode data
    for (uint32_t i = 0; i < inode_size; i++) {
        block[block_offset + i] = ((uint8_t*)inode)[i];
    }

    // Write back the updated block
    write_block(inode_table_block + block_offset / fs_ctx.block_size, block);
    kfree(block);
    return 0;
}
