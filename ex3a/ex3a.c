#include "../shared/constants.h"
#include "../shared/debug.h"
#include "../shared/transformation.h"
#include <stdio.h>
#include <string.h>

void invTransformAndPrint(char *title, unsigned char *src,
                              unsigned char *key, int num) {
    unsigned char state[Nk * Nb];
    unsigned char w[Nk * Nb * (Nr + 1)];

    // Init
    memcpy(state, src, sizeof(state));
    KeyExpansion(w, key);

    // Transform
    for (int i = 0; i < num; i++) {
        invMixColumns(state, state);
        invShiftRows(state, state);
        invSubBytes(state, state);
        AddRoundKey(state, state, w + (num - i - 1) * 16);
    }
    // Print
    printAsAESState(title, state);
}

int main() {
    /* clang-format off */
    unsigned char PT1[16] = {0x00, 0x00, 0x00, 0x00, 
                             0x00, 0x00, 0x00, 0x00,
                             0x00, 0x00, 0x00, 0x00, 
                             0x00, 0x00, 0x00, 0x00};
    unsigned char PT2[16] = {0x01, 0x00, 0x00, 0x00, 
                             0x00, 0x00, 0x00, 0x00,
                             0x00, 0x00, 0x00, 0x00, 
                             0x00, 0x00, 0x00, 0x00};
    unsigned char Key[16] = {0x00, 0x01, 0x02, 0x03, 
                             0x04, 0x05, 0x06, 0x07,
                             0x08, 0x09, 0x0a, 0x0b, 
                             0x0c, 0x0d, 0x0e, 0x0f};
    /* clang-format on */

    invTransformAndPrint("PT1", PT1, Key, 2);
    invTransformAndPrint("PT2", PT2, Key, 2);

    return 0;
}