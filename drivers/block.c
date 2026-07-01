#include "block.h"

static uint8_t block_bitmap[MAX_BLOCKS]; // 0 = livre e 1 = ocupado

/* Limpa o bitmap de blocos */
void block_init(void) {
    uint32_t indice_bloco;

    for (indice_bloco = 0; indice_bloco < MAX_BLOCKS; indice_bloco++) {
        block_bitmap[indice_bloco] = 0;
    }
}

/* Procura um bloco livre no bitmap */
int block_alloc(void) {
    uint32_t indice_bloco;

    for (indice_bloco = 0; indice_bloco < MAX_BLOCKS; indice_bloco++) {
        if (block_bitmap[indice_bloco] == 0) {
            block_bitmap[indice_bloco] = 1;
            return indice_bloco;
        }
    }

    return -1; // erro de alocação, não há blocos livres
}

/* Libera um bloco */
void block_free(uint32_t block) {
    if (block >= MAX_BLOCKS)
        return;

    block_bitmap[block] = 0;
}