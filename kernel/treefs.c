#include "treefs.h"
#include "block.h"
#include "inode.h"
#include "string.h"
#include "timestamp.h"
#include "uart.h"
#define NULL ((void *)0)

static superblock_t superblock; // superbloco do sistema
static diretorio_t diretorios[MAX_INODES]; // um diretório para cada inode
file_t fd_table[MAX_OPEN_FILES]; 

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

    // Preparar estrutura para escrita futura.

    int fd_livre = -1;
    
    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        if (fd_table[i].in_use == 0) {
            fd_livre = i; 
            break;
        }
    }

    if (fd_livre == -1) {
        uart_print("Tabela de arquivos abertos cheia.\n");
        return -1; 
    }

    fd_table[fd_livre].in_use = 1;
    fd_table[fd_livre].inode = novo_inode;
    
    return fd_livre;
}

/* Remove um arquivo do sistema de arquivos */
int unlink(const char *path) {
    char caminho_pai[128];
    char nome[32];

    inode_t *inode_pai;
    inode_t *inode_alvo;

    uint32_t indice_pai;
    uint32_t indice_alvo;
    int indice_entrada;
    uint32_t indice;

    if (path == 0 || path[0] == '\0')
        return -1;

    separa_caminho(path, caminho_pai, nome);
    inode_pai = path_lookup(caminho_pai);

    if (inode_pai == 0)
        return -1;

    if (inode_pai->type != FS_DIR)
        return -1;

    indice_pai = inode_index(inode_pai);
    indice_entrada = procura_entrada(&diretorios[indice_pai], nome);

    if (indice_entrada == -1)
        return -1;

    indice_alvo = diretorios[indice_pai].entradas[indice_entrada].inode;
    inode_alvo = &inode_table[indice_alvo];

    if (inode_alvo->type != FS_FILE)
        return -1;

    for (indice = indice_entrada; indice + 1 < diretorios[indice_pai].quantidade; indice++) {
        diretorios[indice_pai].entradas[indice] = diretorios[indice_pai].entradas[indice + 1];
    }

    uint32_t qtd_blocos = 0;
    
    if (inode_alvo->size > 0) {
        qtd_blocos = (inode_alvo->size - 1) / BLOCK_SIZE + 1; 
    }

    for (uint32_t i = 0; i < qtd_blocos; i++) {
        block_free(inode_alvo->blocks[i]);
    }

    diretorios[indice_pai].quantidade--;
    inode_free(indice_alvo);

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

    if (dir_atual->quantidade == 0) {
        uart_print("Diretorio vazio. Finalizando...\n");
    }else{
        for(int i = 0; i < dir_atual->quantidade; i++){
            uart_print(dir_atual->entradas[i].name);
            uart_print("\n");
        }
    }

    return 0;
}

/* Escrita de Arquivos */
int write(int fd, const void *buf, uint32_t size){

    if (fd < 0 || fd >= MAX_OPEN_FILES) {
        uart_print("FD invalido...\n");
        return -1;
    }

    if (fd_table[fd].in_use == 0) {
        uart_print("O arquivo nao esta aberto...\n");
        return -1;
    }

    inode_t *inode = fd_table[fd].inode;

    uint32_t qtd_blocos = (size + superblock.block_size - 1) / superblock.block_size;

    if(qtd_blocos > MAX_BLOCOS_INODE){
        uart_print("A quantidade de blocos necessaria eh maior que a disponivel no Inode\n");
        return -1;
    }

    int indices_blocos[qtd_blocos];

    for (int i = 0; i < qtd_blocos; i++){
        int bloco_livre = block_alloc();

        if (bloco_livre == -1) {
            uart_print("Nao ha blocos livres suficientes no disco!\n");

            if(i == 0){
                return -1; 
            }

            for(int j = 0; j < i; j++){
                block_free(indices_blocos[j]);
            }
            
            return -1;
        }
        indices_blocos[i] = bloco_livre;
    }

    for (int i = 0; i < qtd_blocos; i++) {
        inode->blocks[i] = indices_blocos[i];
    }
    

    uint32_t bytes = size;
    const uint8_t *buffer = (const uint8_t *)buf;  // Valor que vai ser escrito no arquivo

    for (int i = 0; i < qtd_blocos; i++) {
        void *endereco_bloco = block_get_address(inode->blocks[i]);
        
        uint32_t qtd_bytes = (bytes> BLOCK_SIZE) ? BLOCK_SIZE : bytes; // Calcula quanto copiar (512 bytes ou a sobra que for menor que 512)

        memcpy(endereco_bloco, buffer, qtd_bytes); // Copia o buffer para dentro do bloco

        bytes -= qtd_bytes;
        buffer += qtd_bytes;
    }

    inode->size = size;
    inode->modified_at = timestamp(); // quando escreve atualiza a modificação

    return size; // retorna a qtd de bytes que foi escrita
}

/* Leitura de Arquivos */
int read(int fd, void *buf, uint32_t size){

    if (fd < 0 || fd >= MAX_OPEN_FILES) {
        uart_print("FD invalido...\n");
        return -1;
    }

    if (fd_table[fd].in_use == 0) {
        uart_print("O arquivo nao esta aberto...\n");
        return -1;
    }

    inode_t *inode = fd_table[fd].inode;

    inode->accessed_at = timestamp(); // quando faz a leitura atualiza o acesso

    uint32_t bytes_leitura = size;
    
    if(bytes_leitura > inode->size){
        bytes_leitura = inode->size; // Evita a leitura de lixo
    }

    uint32_t total_lido = bytes_leitura;

    uint32_t qtd_blocos = (bytes_leitura + superblock.block_size - 1) / superblock.block_size;

    uint8_t *buffer = (uint8_t *)buf;

    for (int i = 0; i < qtd_blocos; i++) {
        void *endereco_bloco = block_get_address(inode->blocks[i]);
        
        uint32_t qtd_bytes = (bytes_leitura > BLOCK_SIZE) ? BLOCK_SIZE : bytes_leitura; // Calcula quanto copiar (512 bytes ou a sobra que for menor que 512)

        memcpy(buffer, endereco_bloco, qtd_bytes); // Lógica inversa, o destino é a RAM e a origem os blocos fisicos

        bytes_leitura -= qtd_bytes;
        buffer += qtd_bytes;
    }

    return total_lido; // retorna quantos bytes foram lidos
}

/* Exibe os timestamps armazenados em um inode */
void informacoes_timestamp(const char *path) {
    inode_t *inode = path_lookup(path); // localiza o inode correspondente ao caminho

    if (inode == NULL) {
        uart_print("Arquivo nao encontrado para mostrar o timestamp.\n");
        return;
    }

    uart_print("Criado: ");
    uart_print_uint(inode->created_at);
    uart_print("\n");

    uart_print("Modificado: ");
    uart_print_uint(inode->modified_at);
    uart_print("\n");

    uart_print("Ultimo acesso: ");
    uart_print_uint(inode->accessed_at);
    uart_print("\n");
}