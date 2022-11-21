#include <stdio.h>
#include <string.h>
#include "../utils/transformation.h"

void Encrypt(unsigned char *dst, unsigned char *src, unsigned char *init_key) {
    unsigned char key[16];
    unsigned char w[176];
    unsigned char state[16];

    // この関数を実装してください．
	const int Nb = 4;
	const int Nk = 4;
	const int Nr = 10;

    memcpy(key, init_key, sizeof(key));
    memcpy(state, src, sizeof(key));
    KeyExpansion(w, key);

    AddRoundKey(state, state, w);

    for(int i = 1; i < Nr; i++){
        SubBytes(state, state);
        ShiftRows(state, state);
        MixColumns(state, state);
        AddRoundKey(state, state, w + i*Nb*Nk);
    }

    SubBytes(state, state);
    ShiftRows(state, state);
    AddRoundKey(state, state, w + Nr*Nb*Nk);

    memcpy(dst, state, 16);
}

int main() {
    unsigned char P[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    unsigned char C[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    unsigned char Key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

    Encrypt(C, P, Key);
    printf("0x");
    for(int i = 0; i < 16; i++){
        printf("%02x", C[i]);
    }
    printf("\n");

    return 0;
}
