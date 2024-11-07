#ifndef VFS_H
#define VFS_H

#include "../../kernel/types.h"

#define VFS_MAX_FILENAME 255
#define VFS_MAX_PATH 4096

struct vfs_node;

typedef struct vfs_node* (*vfs_mount_func)(const char* device);
typedef uint32_t (*vfs_read_func)(struct vfs_node*, uint32_t offset, uint32_t size, uint8_t* buffer);
typedef uint32_t (*vfs_write_func)(struct vfs_node*, uint32_t offset, uint32_t size, uint8_t* buffer);
typedef struct vfs_node* (*vfs_finddir_func)(struct vfs_node*, const char* name);

typedef struct vfs_node {
    char name[VFS_MAX_FILENAME];
    uint32_t flags;
    uint32_t inode;
    uint32_t length;
    vfs_read_func read;
    vfs_write_func write;
    vfs_finddir_func finddir;
    struct vfs_node* ptr;
} vfs_node_t;

extern vfs_node_t* vfs_root;

void vfs_init(void);
uint32_t vfs_read(vfs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer);
uint32_t vfs_write(vfs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer);
vfs_node_t* vfs_finddir(vfs_node_t* node, const char* name);
void vfs_mount(const char* path, vfs_node_t* node);

#endif // VFS_H
