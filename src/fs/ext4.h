#ifndef EXT4_H
#define EXT4_H

#include "../kernel/types.h"

// EXT4 Magic Number
#define EXT4_SUPER_MAGIC    0xEF53

// EXT4 Special inode numbers
#define EXT4_ROOT_INO       2
#define EXT4_USR_QUOTA_INO  3
#define EXT4_GRP_QUOTA_INO  4
#define EXT4_BOOT_LOADER_INO 5
#define EXT4_UNDEL_DIR_INO  6
#define EXT4_JOURNAL_INO    8
#define EXT4_FIRST_INO      11

// Filesystem State Values
#define EXT4_STATE_CLEAN    1
#define EXT4_STATE_ERRORS   2

// Feature Flags
#define EXT4_FEATURE_COMPAT_DIR_PREALLOC   0x0001
#define EXT4_FEATURE_COMPAT_IMAGIC_INODES  0x0002
#define EXT4_FEATURE_COMPAT_HAS_JOURNAL    0x0004
#define EXT4_FEATURE_COMPAT_EXT_ATTR       0x0008
#define EXT4_FEATURE_COMPAT_RESIZE_INODE   0x0010
#define EXT4_FEATURE_COMPAT_DIR_INDEX      0x0020

// Superblock structure
struct ext4_superblock {
    uint32_t s_inodes_count;        // Inodes count
    uint32_t s_blocks_count_lo;     // Blocks count
    uint32_t s_r_blocks_count_lo;   // Reserved blocks count
    uint32_t s_free_blocks_count_lo;// Free blocks count
    uint32_t s_free_inodes_count;   // Free inodes count
    uint32_t s_first_data_block;    // First Data Block
    uint32_t s_log_block_size;      // Block size
    uint32_t s_log_cluster_size;    // Cluster size
    uint32_t s_blocks_per_group;    // Blocks per group
    uint32_t s_clusters_per_group;  // Clusters per group
    uint32_t s_inodes_per_group;    // Inodes per group
    uint32_t s_mtime;              // Mount time
    uint32_t s_wtime;              // Write time
    uint16_t s_mnt_count;          // Mount count
    uint16_t s_max_mnt_count;      // Maximal mount count
    uint16_t s_magic;              // Magic signature
    uint16_t s_state;              // File system state
    uint16_t s_errors;             // Behaviour when detecting errors
    uint16_t s_minor_rev_level;    // Minor revision level
    uint32_t s_lastcheck;          // Time of last check
    uint32_t s_checkinterval;      // Maximum time between checks
    uint32_t s_creator_os;         // OS
    uint32_t s_rev_level;          // Revision level
    uint16_t s_def_resuid;         // Default uid for reserved blocks
    uint16_t s_def_resgid;         // Default gid for reserved blocks
} __attribute__((packed));

struct ext4_group_desc {
    uint32_t bg_block_bitmap_lo;      // Blocks bitmap block
    uint32_t bg_inode_bitmap_lo;      // Inodes bitmap block
    uint32_t bg_inode_table;          // Inodes table block
    uint16_t bg_free_blocks_count_lo; // Free blocks count
    uint16_t bg_free_inodes_count_lo; // Free inodes count
    uint16_t bg_used_dirs_count_lo;   // Directories count
    uint16_t bg_flags;                // EXT4_BG_flags (Group flags)
    uint32_t bg_exclude_bitmap_lo;    // Exclude bitmap for snapshots
    uint16_t bg_block_bitmap_csum_lo; // crc32c(s_uuid+grp_num+bitmap) LE
    uint16_t bg_inode_bitmap_csum_lo; // crc32c(s_uuid+grp_num+bitmap) LE
    uint16_t bg_itable_unused_lo;     // Unused inodes count
    uint16_t bg_checksum;             // Group descriptor checksum
} __attribute__((packed));

// Inode structure
struct ext4_inode {
    uint16_t i_mode;               // File mode
    uint16_t i_uid;                // Lower 16 bits of Owner Uid
    uint32_t i_size_lo;            // Size in bytes
    uint32_t i_atime;              // Access time
    uint32_t i_ctime;              // Creation time
    uint32_t i_mtime;              // Modification time
    uint32_t i_dtime;              // Deletion Time
    uint16_t i_gid;                // Lower 16 bits of Group Id
    uint16_t i_links_count;        // Links count
    uint32_t i_blocks_lo;          // Blocks count
    uint32_t i_flags;              // File flags
    uint32_t i_block[15];          // Pointers to blocks
    uint32_t i_generation;         // File version (for NFS)
    uint32_t i_file_acl_lo;        // File ACL
    uint32_t i_size_high;          // High 32 bits of size
} __attribute__((packed));

// Function declarations
int ext4_init(void);
int ext4_mount(const char* device);
int ext4_read_superblock(struct ext4_superblock* sb);
int ext4_write_superblock(const struct ext4_superblock* sb);
int ext4_read_inode(uint32_t inode_num, struct ext4_inode* inode);
int ext4_write_inode(uint32_t inode_num, const struct ext4_inode* inode);

#endif
