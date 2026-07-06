#include "timestamp.h"

static uint32_t contador = 0;

/* Incrementa o contador global de timestamps e retorna seu valor */
uint32_t timestamp(void) {
    return ++contador;
}