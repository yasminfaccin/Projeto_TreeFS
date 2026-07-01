#include <stdint.h>

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