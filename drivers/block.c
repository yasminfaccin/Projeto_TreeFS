#include "block.h"
#include "uart.h"
#include <stddef.h>

static uint8_t block_bitmap[MAX_BLOCKS]; // 0 = livre e 1 = ocupado
static uint8_t disk[MAX_BLOCKS][BLOCK_SIZE];

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
            uart_print("Bloco fisico alocado: ");
            uart_print_uint(indice_bloco); 
            uart_print("\n");
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
    uart_print("Bloco fisico liberado: ");
    uart_print_uint(block);
    uart_print("\n");
}

void* block_get_address(uint32_t block_num) {
    if (block_num >= MAX_BLOCKS) return NULL;
    return disk[block_num];
}