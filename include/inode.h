#pragma once

#include <stdint.h>

#define MAX_INODES 128
#define MAX_BLOCOS_INODE 8
#define FS_FILE 1
#define FS_DIR  2

typedef struct {
    uint32_t size;
    uint16_t type;
    uint16_t links;
    uint32_t blocks[MAX_BLOCOS_INODE];
} inode_t;

void inode_init(void);

inode_t *inode_alloc(void);

void inode_free(uint32_t inode);

uint32_t inode_index(inode_t *inode);