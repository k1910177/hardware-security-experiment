#include "constants.h"
#include <stdio.h>

void printAsHex(char *title, unsigned char *src) {
    printf("%s: 0x", title);
    for (int i = 0; i < Nk * Nb; i++) {
        printf("%02x", src[i]);
    }
    printf("\n");
}

void printAsAESState(char *title, unsigned char *src) {
    printf("=== %s ===\n", title);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%02x ", src[i + 4 * j]);
        }
        printf("\n");
    }
    printf("\n");
}

void printAsTrAESState(char *title, unsigned char *src) {
    printf("=== %s ===\n", title);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%02x ", src[4 * i + j]);
        }
        printf("\n");
    }
    printf("\n");
}

void printAsUCharArray(char *title, unsigned char *src) {
    printf("%s: {", title);
    for (int i = 0; i < Nk * Nb; i++) {
        printf("0x%02x", src[i]);
        if (i < Nk * Nb - 1) {
            printf(", ");
        }
    }
    printf("}\n");
}
