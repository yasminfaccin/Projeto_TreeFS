#pragma once

#include <stdint.h>
#include "inode.h"

#define TREEFS_MAGIC 0x54524653 // "TRFS" em ASCII
#define MAX_ENTRADAS_DIRETORIO 32 
#define MAX_OPEN_FILES 128

/* Superbloco */
typedef struct {
    uint32_t magic;
    uint32_t total_blocks;
    uint32_t total_inodes;
    uint32_t block_size;
    uint32_t root_inode;
} superblock_t;

/* Entrada de um diretório */
typedef struct {
    uint32_t inode;
    char name[32];
} dir_entry_t;

/* Conteúdo de um diretório */
typedef struct {
    dir_entry_t entradas[MAX_ENTRADAS_DIRETORIO];
    uint32_t quantidade;
} diretorio_t;

typedef struct {
    inode_t *inode;     
    uint32_t offset;     
    int in_use;  // (0 = livre, 1 = ocupado)
} file_t;

extern file_t fd_table[MAX_OPEN_FILES];

int fs_init(void);

inode_t *path_lookup(const char *path);

int ls(const char *path);

int mkdir(const char *path);

int create(const char *path);

int unlink(const char *path);

int write(int fd, const void *buf, uint32_t size);

int read(int fd, void *buf, uint32_t size);

void informacoes_timestamp(const char *path);