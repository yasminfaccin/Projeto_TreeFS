#include "treefs.h"
#include "block.h"
#include "inode.h"
#include "string.h"

static superblock_t superblock; // superbloco do sistema
static diretorio_t diretorios[MAX_INODES]; // um diretório para cada inode

/* Inicialização do sistema de arquivos */
int fs_init(void) {
    inode_t *raiz;
    inode_t *home;
    inode_t *tmp;
    inode_t *bin;

    uint32_t indice_raiz;
    uint32_t indice_home;
    uint32_t indice_tmp;
    uint32_t indice_bin;

    uint32_t indice_entrada;
    indice_entrada = diretorios[indice_raiz].quantidade;

    // inicializa estruturas básicas
    inode_init();
    block_init();

    // inicializa o superbloco
    superblock.magic = TREEFS_MAGIC;
    superblock.total_blocks = MAX_BLOCKS;
    superblock.total_inodes = MAX_INODES;
    superblock.block_size = BLOCK_SIZE;

    // cria o diretório raiz (/)
    raiz = inode_alloc();

    if (raiz == 0)
        return -1;

    raiz->type = FS_DIR;
    indice_raiz = inode_index(raiz);
    diretorios[indice_raiz].quantidade = 0;

    // cria /home
    home = inode_alloc();

    if (home == 0)
        return -1;

    home->type = FS_DIR;
    indice_home = inode_index(home);
    diretorios[indice_home].quantidade = 0;
    diretorios[indice_raiz].entradas[indice_entrada].inode = indice_home;
    strcpy(diretorios[indice_raiz].entradas[indice_entrada].name, "home");
    diretorios[indice_raiz].quantidade++;

    // cria /tmp
    tmp = inode_alloc();

    if (tmp == 0)
        return -1;

    tmp->type = FS_DIR;
    indice_tmp = inode_index(tmp);
    diretorios[indice_tmp].quantidade = 0;
    diretorios[indice_raiz].entradas[indice_entrada].inode = indice_tmp;
    strcpy(diretorios[indice_raiz].entradas[indice_entrada].name, "tmp");
    diretorios[indice_raiz].quantidade++;

    // cria /bin
    bin = inode_alloc();

    if (bin == 0)
        return -1;

    bin->type = FS_DIR;
    indice_bin = inode_index(bin);
    diretorios[indice_bin].quantidade = 0;
    diretorios[indice_raiz].entradas[indice_entrada].inode = indice_bin;
    strcpy(diretorios[indice_raiz].entradas[indice_entrada].name, "bin");
    diretorios[indice_raiz].quantidade++;

    return 0;
}

/* Dependem de path_lookup()!! */
int mkdir(const char *path) {
    (void)path;
    return -1;
}

int create(const char *path) {
    (void)path;
    return -1;
}

inode_t *path_lookup(const char *path) {
    (void)path;
    return 0;
}