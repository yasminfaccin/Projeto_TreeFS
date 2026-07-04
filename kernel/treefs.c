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

/* Separa um caminho em diretório pai e nome */
static void separa_caminho(const char *path, char *pai, char *nome) {
    uint32_t ultimo_separador = 0;
    uint32_t indice;

    // procura onde está a última '/'
    for (indice = 0; path[indice] != '\0'; indice++) {
        if (path[indice] == '/') {
            ultimo_separador = indice;
        }
    }

    // copia o diretório pai
    if (ultimo_separador == 0) {
        pai[0] = '/';
        pai[1] = '\0';

    } else {
        for (indice = 0; indice < ultimo_separador; indice++) {
            pai[indice] = path[indice];
        }
        pai[ultimo_separador] = '\0';
    }

    // copia apenas o nome
    indice = 0;

    while (path[ultimo_separador + 1] != '\0') {
        nome[indice++] = path[ultimo_separador + 1];
        ultimo_separador++;
    }

    nome[indice] = '\0'; // garante que a string termine com '\0'
}

/* Procura uma entrada pelo nome dentro de um diretório */
static int procura_entrada(diretorio_t *diretorio, const char *nome) {
    uint32_t indice_entrada;

    for (indice_entrada = 0;
         indice_entrada < diretorio->quantidade;
         indice_entrada++) {

        if (strcmp(diretorio->entradas[indice_entrada].name, nome) == 0) {
            return indice_entrada;
        }
    }

    return -1;
}

/* Cria um novo diretório */
int mkdir(const char *path) {
    char caminho_pai[128];
    char nome[32];

    inode_t *inode_pai;
    inode_t *novo_inode;

    uint32_t indice_pai;
    uint32_t indice_novo;
    uint32_t indice_entrada;

    separa_caminho(path, caminho_pai, nome); // separa o caminho

    inode_pai = path_lookup(caminho_pai); // acha o inode do diretório pai

    if (inode_pai == 0)
        return -1;

    if (inode_pai->type != FS_DIR)
        return -1;

    indice_pai = inode_index(inode_pai); // pega o índice do inode pai na tabela de inodes

    // para verificar a duplicidade
    if (procura_entrada(&diretorios[indice_pai], nome) != -1)
        return -1;

    // ve se o diretório pai tem espaço para mais entradas
    if (diretorios[indice_pai].quantidade >= MAX_ENTRADAS_DIRETORIO)
        return -1;

    novo_inode = inode_alloc();

    if (novo_inode == 0)
        return -1;

    novo_inode->type = FS_DIR;
    indice_novo = inode_index(novo_inode);
    diretorios[indice_novo].quantidade = 0;

    indice_entrada = diretorios[indice_pai].quantidade;
    diretorios[indice_pai].entradas[indice_entrada].inode = indice_novo;
    strcpy(diretorios[indice_pai].entradas[indice_entrada].name, nome);
    diretorios[indice_pai].quantidade++;

    return 0;
}

/* Cria um novo arquivo */
int create(const char *path) {
    char caminho_pai[128];
    char nome[32];

    inode_t *inode_pai;
    inode_t *novo_inode;

    uint32_t indice_pai;
    uint32_t indice_novo;
    uint32_t indice_entrada;

    separa_caminho(path, caminho_pai, nome); // separa o caminho
    inode_pai = path_lookup(caminho_pai); // acha o inode do diretório pai

    if (inode_pai == 0) 
        return -1;

    if (inode_pai->type != FS_DIR)
        return -1;

    indice_pai = inode_index(inode_pai);

    // para verificar a duplicidade
    if (procura_entrada(&diretorios[indice_pai], nome) != -1)
        return -1;
    
    // ve se o diretório pai tem espaço para mais entradas
    if (diretorios[indice_pai].quantidade >= MAX_ENTRADAS_DIRETORIO)
        return -1;

    novo_inode = inode_alloc();

    if (novo_inode == 0)
        return -1;

    // inicializa o novo inode como arquivo
    novo_inode->type = FS_FILE;
    novo_inode->size = 0;
    novo_inode->links = 1;

    indice_novo = inode_index(novo_inode);
    indice_entrada = diretorios[indice_pai].quantidade;
    diretorios[indice_pai].entradas[indice_entrada].inode = indice_novo;

    strcpy(diretorios[indice_pai].entradas[indice_entrada].name, nome);
    diretorios[indice_pai].quantidade++;

    return 0;
}

/* Resolução de Caminhos */
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

/* Lista o conteúdo de um diretório */
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