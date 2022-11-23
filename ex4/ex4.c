#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "../utils/constants.h"
#include "../utils/debug.h"
#include "../utils/parser.h"
#include "../utils/transformation.h"

void Transform(unsigned char *state, unsigned char *rKey) {
    AddRoundKey(state, state, rKey);
    SubBytes(state, state);
    ShiftRows(state, state);
    MixColumns(state, state);
}

bool isOneByteDiff(unsigned char *x, unsigned char *y, int type) {
    int diffCount = 0;
    for (int i = 0; i < 4; i++) {
        int index = 4 * type + i;
        if (x[index] != y[index]) diffCount++;
        if (diffCount > 1) return false;
    }
    return diffCount == 1 ? true : false;
}

void combinePartialKeys(unsigned char *dst, unsigned char keys[4][16]) {
    for (int i = 0; i < 16; i++) {
        dst[i] = keys[0][i] | keys[1][i] | keys[2][i] | keys[3][i];
    }
}

typedef struct arg_struct {
    unsigned char *dst;
    unsigned char *pt1;
    unsigned char *pt2;
    int type;
} arg_struct;

void *findPartialKey(void *threadArg) {
    arg_struct *arg = (arg_struct *)threadArg;

    unsigned char key[16];
    unsigned char state1[Nk * Nb];
    unsigned char state2[Nk * Nb];

    memset(key, 0, sizeof(key));

    for (int k0 = 0; k0 < 256; k0++) {
        printf("(type%d) %d / 255\n", arg->type, k0); // print progress

        switch (arg->type) {
        case 0: key[0] = k0; break;
        case 1: key[4] = k0; break;
        case 2: key[8] = k0; break;
        case 3: key[12] = k0; break;
        }

        for (int k1 = 0; k1 < 256; k1++) {
            switch (arg->type) {
            case 0: key[5] = k1; break;
            case 1: key[9] = k1; break;
            case 2: key[13] = k1; break;
            case 3: key[1] = k1; break;
            }

            for (int k2 = 0; k2 < 256; k2++) {
                switch (arg->type) {
                case 0: key[10] = k2; break;
                case 1: key[14] = k2; break;
                case 2: key[2] = k2; break;
                case 3: key[6] = k2; break;
                }

                for (int k3 = 0; k3 < 256; k3++) {
                    switch (arg->type) {
                    case 0: key[15] = k3; break;
                    case 1: key[3] = k3; break;
                    case 2: key[7] = k3; break;
                    case 3: key[11] = k3; break;
                    }

                    memcpy(state1, arg->pt1, sizeof(state1));
                    memcpy(state2, arg->pt2, sizeof(state2));

                    Transform(state1, key);
                    Transform(state2, key);

                    if (isOneByteDiff(state1, state2, arg->type)) {
                        memcpy(arg->dst, key, sizeof(key));
                        printf("type%d done!\n", arg->type);
                        printAsAESState("key", arg->dst);
                        return NULL;
                    }
                }
            }
        }
    }
}

int main() {
    unsigned char PT1[4][16];
    unsigned char PT2[4][16];
    unsigned char pKeys[4][16];
    unsigned char Key[16];
    pthread_t threads[4];
    arg_struct thread_params[4];

    const char *files[4] = {"pt/8", "pt/10", "pt/1", "pt/4"};

    for (int i = 0; i < 4; i++) {
        thread_params[i].dst = pKeys[i];
        thread_params[i].pt1 = PT1[i];
        thread_params[i].pt2 = PT2[i];
        thread_params[i].type = i;

        readfile(files[i], PT1[i], PT2[i]);
        pthread_create(&threads[i], NULL, findPartialKey,
                       (void *)&thread_params[i]);
    }

    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }

    combinePartialKeys(Key, pKeys);
    printAsHex("key", Key);

    return 0;
}
