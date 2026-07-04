#include "treefs.h"
#include "block.h"
#include "inode.h"
#include "string.h"
#include "uart.h"
#define NULL ((void *)0)

static superblock_t superblock; // superbloco do sistema
static diretorio_t diretorios[MAX_INODES]; // um diretório para cada inode

/* Inicialização do sistema de arquivos */
int fs_init(void) {
    inode_t *raiz;
    inode_t *home;
    inode_t *tmp;
    inode_t *bin;

    uint32_t indice_raiz = 0;
    uint32_t indice_home;
    uint32_t indice_tmp;
    uint32_t indice_bin;

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
    superblock.root_inode = indice_raiz; // Armazena o indice da raiz para podermos usar no path_loockup()
    diretorios[indice_raiz].quantidade = 0;

    // cria /home
    home = inode_alloc();

    if (home == 0)
        return -1;

    home->type = FS_DIR;
    indice_home = inode_index(home);
    diretorios[indice_home].quantidade = 0;

    uint32_t espaco_vazio = diretorios[indice_raiz].quantidade;

    diretorios[indice_raiz].entradas[espaco_vazio].inode = indice_home;
    strcpy(diretorios[indice_raiz].entradas[espaco_vazio].name, "home");
    diretorios[indice_raiz].quantidade++;

    // cria /tmp
    tmp = inode_alloc();

    if (tmp == 0)
        return -1;

    tmp->type = FS_DIR;
    indice_tmp = inode_index(tmp);
    diretorios[indice_tmp].quantidade = 0;

    espaco_vazio = diretorios[indice_raiz].quantidade;

    diretorios[indice_raiz].entradas[espaco_vazio].inode = indice_tmp;
    strcpy(diretorios[indice_raiz].entradas[espaco_vazio].name, "tmp");
    diretorios[indice_raiz].quantidade++;

    // cria /bin
    bin = inode_alloc();

    if (bin == 0)
        return -1;

    bin->type = FS_DIR;
    indice_bin = inode_index(bin);
    diretorios[indice_bin].quantidade = 0;

    espaco_vazio = diretorios[indice_raiz].quantidade;

    diretorios[indice_raiz].entradas[espaco_vazio].inode = indice_bin;
    strcpy(diretorios[indice_raiz].entradas[espaco_vazio].name, "bin");
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
    diretorio_t *dir_atual = &diretorios[superblock.root_inode]; //pega o diretorio raiz (C:\ do Windows)

    char caminho_temp[256];
    strcpy(caminho_temp, path); // Faz uma copia do path para podermos fatia-lo e não modificar o path original

    char *lasts;
    char *nome_atual = strtok_r(caminho_temp,&lasts);

    uint32_t flag = 0;

    uint32_t proximo_inode = superblock.root_inode;

    while(nome_atual != NULL){

        for(int i = 0; i < dir_atual->quantidade; i++){
            if(strcmp(dir_atual->entradas[i].name, nome_atual) == 0){
                flag = 1;
                proximo_inode = dir_atual->entradas[i].inode;
                dir_atual = &diretorios[proximo_inode];
                break;
            }
        }

        if(flag == 0){
            return NULL; //Não achou o dir
        }else{
            flag = 0;
        }
        nome_atual = strtok_r(NULL,&lasts);
    }

    return &inode_table[proximo_inode];
}

int ls(const char *path){
    inode_t *inode = path_lookup(path);

    if(inode == NULL){
        uart_print("O caminho indicado nao existe. Finalizando...");
        return 1;
    }

    if(inode->type == 1){
        uart_print("Isso nao eh um diretorio. Finalizando...\n");
        return 1;
    }

    uint32_t indice_inode = inode_index(inode);
    diretorio_t *dir_atual = &diretorios[indice_inode];

    for(int i = 0; i < dir_atual->quantidade; i++){
        uart_print(dir_atual->entradas[i].name);
        uart_print("\n");
    }

    return 0;
}