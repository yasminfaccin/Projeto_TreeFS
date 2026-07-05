#pragma once

#include <stdint.h>

#define MAX_BLOCKS 1024
#define BLOCK_SIZE 512

void block_init(void);

int block_alloc(void);

void block_free(uint32_t block);

void* block_get_address(uint32_t block_num);