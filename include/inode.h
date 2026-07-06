#pragma once

#include <stdint.h>

#define MAX_INODES 128
#define MAX_BLOCOS_INODE 8
#define FS_FILE 1
#define FS_DIR  2

#define PERM_READ  4  // Leitura
#define PERM_WRITE 2  // Escritas
#define PERM_EXEC  1  // Execução/Entrar nas pastas

typedef struct {
    uint32_t size;
    uint16_t type;
    uint16_t links;
    uint32_t blocks[MAX_BLOCOS_INODE];
    // timestamp
    uint32_t created_at;
    uint32_t modified_at;
    uint32_t accessed_at;
    // permissoes
    uint8_t permissions;
} inode_t;

extern inode_t inode_table[];

void inode_init(void);

inode_t *inode_alloc(void);

void inode_free(uint32_t inode);

uint32_t inode_index(inode_t *inode);