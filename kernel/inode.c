#include "inode.h"
#include "uart.h"

static uint8_t inode_bitmap[MAX_INODES]; // 0 = livre e 1 = ocupado
  inode_t inode_table[MAX_INODES];

/* Limpa o bitmap e a tabela de inodes */
void inode_init(void) {
    uint32_t indice_inode, indice_bloco;

    for (indice_inode = 0; indice_inode < MAX_INODES; indice_inode++) {
        inode_bitmap[indice_inode] = 0;

        inode_table[indice_inode].size = 0;
        inode_table[indice_inode].type = 0;
        inode_table[indice_inode].links = 0;

        // zera os blocos apontados por este inode
        for (indice_bloco = 0; indice_bloco < MAX_BLOCOS_INODE; indice_bloco++) {
            inode_table[indice_inode].blocks[indice_bloco] = 0;
        }
    }
}

/* Procura um inode livre no bitmap */
inode_t *inode_alloc(void) {
    uint32_t indice_inode, indice_bloco;

    for (indice_inode = 0; indice_inode < MAX_INODES; indice_inode++) {
        if (inode_bitmap[indice_inode] == 0) {
            inode_bitmap[indice_inode] = 1;

            inode_table[indice_inode].size = 0;
            inode_table[indice_inode].type = 0;
            inode_table[indice_inode].links = 1;

            // garante que os blocos comecem limpos
            for (indice_bloco = 0; indice_bloco < MAX_BLOCOS_INODE; indice_bloco++) {
                inode_table[indice_inode].blocks[indice_bloco] = 0;
            }

            uart_print("Inode alocado: ");
            uart_print_uint(indice_inode);
            uart_print("\n");

            return &inode_table[indice_inode];
        }
    }

    return 0;
}

/* Marca o inode como livre e limpa seus campos */
void inode_free(uint32_t inode) {
    uint32_t indice_bloco;

    if (inode >= MAX_INODES)
        return;

    inode_bitmap[inode] = 0;

    inode_table[inode].size = 0;
    inode_table[inode].type = 0;
    inode_table[inode].links = 0;

    // limpa a lista de blocos associada ao inode
    for (indice_bloco = 0; indice_bloco < MAX_BLOCOS_INODE; indice_bloco++) {
        inode_table[inode].blocks[indice_bloco] = 0;
    }
    uart_print("Inode liberado: ");
    uart_print_uint(inode);
    uart_print("\n");
}

/* Retorna o índice do inode na tabela de inodes */
uint32_t inode_index(inode_t *inode) {
    return (uint32_t)(inode - inode_table);
}