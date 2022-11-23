#include "../utils/constants.h"
#include "../utils/debug.h"
#include "../utils/transformation.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

void invTransformOnce(unsigned char *state, unsigned char *key) {
    unsigned char w[Nk * Nb * (Nr + 1)];

    // Init
    KeyExpansion(w, key);

    // Transform
    invMixColumns(state, state);
    invShiftRows(state, state);
    invSubBytes(state, state);
    AddRoundKey(state, state, w);
}

void TransformOnce(unsigned char *state, unsigned char *key) {
    unsigned char w[Nk * Nb * (Nr + 1)];

    // Init
    KeyExpansion(w, key);

    // Transform
    AddRoundKey(state, state, w);
    SubBytes(state, state);
    ShiftRows(state, state);
    MixColumns(state, state);
}

int main() {
    /* clang-format off */

    // PT1 round1 state in AES state format
    unsigned char ST1[16] = {0x3d, 0x1f, 0x9e, 0x03, 
                             0xc0, 0xb1, 0xa4, 0x02,
                             0xc0, 0xff, 0xfa, 0x8b, 
                             0xe1, 0x11, 0x58, 0x55};

    // PT2 round1 state in AES state format
    unsigned char ST2[16] = {0x3d, 0x1f, 0x9e, 0x03, 
                             0xc0, 0xb1, 0xa4, 0x02,
                             0xc0, 0xff, 0xfa, 0x8b, 
                             0x1f, 0x11, 0x58, 0x55};
                            // ^--- 

    // True key in AES state format
    unsigned char TrueKey[16] = {0x00, 0x01, 0x02, 0x03, 
                                 0x04, 0x05, 0x06, 0x07,
                                 0x08, 0x09, 0x0a, 0x0b, 
                                 0x0c, 0x0d, 0x0e, 0x0f};

    // Test key in AES state format
    unsigned char TestKey[16] = {0x00, 0x01, 0x02, 0x00, 
                                 0x00, 0x00, 0x06, 0x07,
                                 0x08, 0x00, 0x00, 0x0b, 
                                 0x0c, 0x0d, 0x00, 0x00};
    /* clang-format on */

    // AES state => array format
    Transpose(ST1);
    Transpose(ST2);
    Transpose(TrueKey);
    Transpose(TestKey);

    // Calculate 'the given' plain text
    printf("* PLAIN TEXT *\n");
    invTransformOnce(ST1, TrueKey);
    invTransformOnce(ST2, TrueKey);
    printAsAESState("PT1", ST1);
    printAsAESState("PT2", ST2);

    // Transform using TestKey  
    printf("*** RESULT ***\n");
    TransformOnce(ST1, TestKey);
    TransformOnce(ST2, TestKey);
    printAsAESState("ST1", ST1);
    printAsAESState("ST2", ST2);

    return 0;
}
