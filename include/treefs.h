#pragma once

#include <stdint.h>

#define TREEFS_MAGIC 0x54524653

typedef struct {
    uint32_t magic;
    uint32_t total_blocks;
    uint32_t total_inodes;
    uint32_t block_size;

} superblock_t;

int fs_init(void);