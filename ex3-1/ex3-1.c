#include <stdio.h>
#include <string.h>
#include "../utils/transformation.h"

int main() {
    unsigned char PT1[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    unsigned char PT2[16] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    KeyExpansion(w, key);

    AddRoundKey(PT1, PT1, w);

    SubBytes(PT1, PT1);
    ShiftRows(PT1, PT1);
    MixColumns(PT1, PT1);
    printf("PT1: 0x");
    for(int i = 0; i < 16; i++){
        printf("%02x", PT1[i]);
    }
    printf("\n");

    SubBytes(PT2, PT2);
    ShiftRows(PT2, PT2);
    MixColumns(PT2, PT2);
    printf("PT2: 0x");
    for(int i = 0; i < 16; i++){
        printf("%02x", PT2[i]);
    }
    printf("\n");

    return 0;
}
