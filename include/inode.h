#pragma once

#include <stdint.h>

#define MAX_INODES 128
#define FS_FILE 1
#define FS_DIR  2

typedef struct {
    uint32_t size;
    uint16_t type;
    uint16_t links;
    uint32_t blocks[8];
} inode_t;

extern inode_t inode_table[MAX_INODES];

void inode_init(void);

inode_t *inode_alloc(void);

void inode_free(uint32_t inode);