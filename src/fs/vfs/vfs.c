#include "vfs.h"
#include "../../mm/kheap.h"

vfs_node_t* vfs_root = 0;

void vfs_init(void) {
    vfs_root = 0;
}

uint32_t vfs_read(vfs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    if (node && node->read) {
        return node->read(node, offset, size, buffer);
    }
    return 0;
}

uint32_t vfs_write(vfs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    if (node && node->write) {
        return node->write(node, offset, size, buffer);
    }
    return 0;
}

vfs_node_t* vfs_finddir(vfs_node_t* node, const char* name) {
    if (node && node->finddir) {
        return node->finddir(node, name);
    }
    return 0;
}

static int vfs_strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

static char* vfs_strcpy(char* dest, const char* src) {
    char* d = dest;
    while ((*d++ = *src++));
    return dest;
}

void vfs_mount(const char* path, vfs_node_t* node) {
    if (!vfs_root) {
        vfs_root = node;
        return;
    }

    vfs_node_t* parent = vfs_root;
    char* part = (char*)path;
    char name[VFS_MAX_FILENAME];
    int i;

    while (*part) {
        for (i = 0; *part && *part != '/'; part++, i++) {
            name[i] = *part;
        }
        name[i] = 0;

        if (*part == '/') part++;

        vfs_node_t* next = vfs_finddir(parent, name);
        if (!next) {
            // Create a new directory node
            next = (vfs_node_t*)kmalloc(sizeof(vfs_node_t));
            vfs_strcpy(next->name, name);
            next->flags = 0; // Set appropriate flags
            next->read = 0;
            next->write = 0;
            next->finddir = parent->finddir;
            next->ptr = 0;
            // Add to parent's directory (this part depends on your specific implementation)
        }

        parent = next;
    }

    // Mount the node
    parent->ptr = node;
}
