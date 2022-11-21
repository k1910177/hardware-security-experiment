#include "../utils/constants.h"
#include "../utils/transformation.h"
#include <stdio.h>
#include <string.h>

void Encrypt(unsigned char *dst, unsigned char *src, unsigned char *init_key) {
    unsigned char state[Nk * Nb];
    unsigned char key[Nk * Nb];
    unsigned char w[Nk * Nb * (Nr + 1)];

    memcpy(key, init_key, sizeof(key));
    memcpy(state, src, sizeof(state));
    KeyExpansion(w, key);

    AddRoundKey(state, state, w);

    for (int i = 1; i < Nr; i++) {
        SubBytes(state, state);
        ShiftRows(state, state);
        MixColumns(state, state);
        AddRoundKey(state, state, w + i * Nb * Nk);
    }

    SubBytes(state, state);
    ShiftRows(state, state);
    AddRoundKey(state, state, w + Nr * Nb * Nk);

    memcpy(dst, state, sizeof(state));
}

int main() {
    unsigned char P[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    unsigned char C[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    unsigned char Key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                             0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

    Encrypt(C, P, Key);
    printf("0x");
    for (int i = 0; i < 16; i++) {
        printf("%02x", C[i]);
    }
    printf("\n");

    return 0;
}
