#include <stdint.h>
#include "string.h"
#define NULL ((void *)0)

void *memcpy(void *dest, const void *src, uint64_t n) {
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;

    while (n--) {
        *d++ = *s++;
    }

    return dest;
}

/* Preenche n bytes com um valor */
void *memset(void *s, int c, uint64_t n) {
    uint8_t *ponteiro = (uint8_t *)s;

    while (n--) {
        *ponteiro++ = (uint8_t)c;
    }

    return s;
}

/* Compara duas strings */
int strcmp(const char *a, const char *b) {
    while (*a != '\0' && *b != '\0') {
        if (*a != *b) {
            return *a - *b;
        }

        a++;
        b++;
    }

    return *a - *b;
}

/* Copia uma string */
char *strcpy(char *dest, const char *src) {
    char *inicio = dest;

    while (*src != '\0') {
        *dest = *src;
        dest++;
        src++;
    }

    *dest = '\0';

    return inicio;
}

/* Fatia o path */
char *strtok_r(char *s, char **lasts)
{
    char *temp = s;

    // A primeira pasta já foi encontrada, então inicia a partir da prox
    if (temp == NULL) {
        temp = *lasts; 
    }

    // pula os delimitadores iniciais
    while(*temp == '/'){
        temp++;
    }

    // não sobrou nada no path, entao retorna nulo
    if(*temp == '\0'){
        return NULL;
    }
    
    // armazena o inicio da palavra, para saber quando iniciar na prox chamada de função
    char *inicio = temp;

    while (*temp != '/' && *temp != '\0')
    {
        temp++;
    }

    if(*temp == '/'){
        *temp = '\0';
        *lasts = ++temp;
    }else{
        *lasts = temp;
    }

    return inicio;
}