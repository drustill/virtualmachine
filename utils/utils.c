/* utils.c */
/* Credits: https://www.youtube.com/@dr-Jonas-Birch */

/* #include <birchutils.h> */
#include "birchutils.h"

/* @brief memcpy */
void copy(int8 *dst, int8 *src, int16 size) {
    int8 *d, *s;
    int16 n;

    for (n = size, d = dst, s = src; n; n--, d++, s++) {
        *d = *s;
    }

    return;
}

/* @brief Sets a memory region to zero */
void zero(int8 *str, int16 size) {
    int8 *p;
    int16 n;

    for (n = 0, p = str; n < size; n++, p++) {
        *p = 0;
    }

    return;
}

/* @brief prints hex representation */
void printhex(int8 *str, int16 size, int8 delim) {
    int8 *p;
    int16 n;

    for (p = str, n = size; n; n--, p++) {
        printf("%.02x", *p);
        if (delim) {
            printf("%c", delim);
        }
        fflush(stdout);
    }
    printf("\n");

    return;
}
