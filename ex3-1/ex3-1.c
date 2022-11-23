#include "../utils/constants.h"
#include "../utils/debug.h"
#include "../utils/transformation.h"
#include <stdio.h>
#include <string.h>

void TransformOnceAndPrint(char *title, unsigned char *src,
                           unsigned char *key) {
    unsigned char state[Nk * Nb];
    unsigned char w[Nk * Nb * (Nr + 1)];

    // Init
    memcpy(state, src, sizeof(state));
    KeyExpansion(w, key);

    // Transform
    AddRoundKey(state, state, w);
    SubBytes(state, state);
    ShiftRows(state, state);
    MixColumns(state, state);

    // Print
    printAsAESState(title, state);
}

int main() {
    unsigned char PT1[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    unsigned char PT2[16] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    unsigned char Key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                             0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

    TransformOnceAndPrint("PT1", PT1, Key);
    TransformOnceAndPrint("PT2", PT2, Key);

    return 0;
}
