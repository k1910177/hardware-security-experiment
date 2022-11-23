#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "../utils/debug.h"
#include "../utils/parser.h"
#include "../utils/transformation.h"

/**
 *  type0  |  type1  |  type2  |  type3 
 * x 0 0 0 | 0 x 0 0 | 0 0 x 0 | 0 0 0 x
 * 0 x 0 0 | 0 0 x 0 | 0 0 0 x | x 0 0 0
 * 0 0 x 0 | 0 0 0 x | x 0 0 0 | 0 x 0 0
 * 0 0 0 x | x 0 0 0 | 0 x 0 0 | 0 0 x 0 
 */

typedef struct arg_struct {
    unsigned char *dst;
    char **files;
    int type;
    int num;
    int from;
    int to;
} arg_struct;

bool done;
const int threadCount = 8; // Must be a divisor of 256

void transform(unsigned char *state, unsigned char *rKey) {
    AddRoundKey(state, state, rKey);
    SubBytes(state, state);
    ShiftRows(state, state);
    MixColumns(state, state);
}

bool isOneByteOff(unsigned char *x, unsigned char *y, int type) {
    int diffCount = 0;
    for (int i = 0; i < 4; i++) {
        int index = 4 * type + i;
        if (x[index] != y[index]) diffCount++;
        if (diffCount > 1) return false;
    }
    return diffCount == 1 ? true : false;
}

void *findPartialKey(void *threadArg) {
    // Parse arguments
    arg_struct *arg = (arg_struct *)threadArg;

    unsigned char key[16];
    unsigned char state1[16];
    unsigned char state2[16];
    unsigned char pt1[arg->num][16];
    unsigned char pt2[arg->num][16];

    memset(key, 0, sizeof(key));
    for (int i = 0; i < arg->num; i++) {
        readfile(arg->files[i], pt1[i], pt2[i]);
    }

    for (int k0 = arg->from; k0 < arg->to; k0++) {
        // Prints progress
        printf("%d / %d\n", k0 % (256 / threadCount), (256 / threadCount));

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
                    if (done) return NULL;

                    switch (arg->type) {
                    case 0: key[15] = k3; break;
                    case 1: key[3] = k3; break;
                    case 2: key[7] = k3; break;
                    case 3: key[11] = k3; break;
                    }

                    int fileIndex;
                    for (fileIndex = 0; fileIndex < arg->num; fileIndex++) {
                        memcpy(state1, pt1[fileIndex], sizeof(state1));
                        memcpy(state2, pt2[fileIndex], sizeof(state2));

                        transform(state1, key);
                        transform(state2, key);

                        if (!isOneByteOff(state1, state2, arg->type)) break;
                    }

                    if (fileIndex == arg->num) {
                        done = true;
                        memcpy(arg->dst, key, sizeof(key));
                        printf("type%d done!\n", arg->type);
                        printAsAESState("key", arg->dst);
                        return NULL;
                    }
                }
            }
        }
    }
    return NULL;
}

void combinePartialKeys(unsigned char *dst, unsigned char pKeys[4][16]) {
    for (int i = 0; i < 16; i++) {
        dst[i] = pKeys[0][i] | pKeys[1][i] | pKeys[2][i] | pKeys[3][i];
    }
}

int main() {
    // Partial keys
    unsigned char pKeys[4][16];
    // Final key
    unsigned char Key[16];

    /* clang-format off */
    char *files[4][10] = {{"pt/8", "pt/9", "pt/12", "pt/15"},                            // type 0
                          {"pt/10", "pt/14", "pt/20"},                                   // type 1
                          {"pt/1", "pt/2", "pt/3", "pt/11", "pt/13", "pt/17", "pt/18"},  // type 2
                          {"pt/4", "pt/5", "pt/6", "pt/7", "pt/16", "pt/19"}};           // type 3
    const int fileCounts[4] = {4, 3, 7, 6};
    /* clang-format on */

    for (int type = 0; type < 4; type++) {
        pthread_t threads[threadCount];
        arg_struct threadParams[threadCount];

        for (int i = 0; i < threadCount; i++) {
            threadParams[i].dst = pKeys[type];
            threadParams[i].files = files[type];
            threadParams[i].num = fileCounts[type];
            threadParams[i].from = i * 256 / threadCount;
            threadParams[i].to = i * 256 / threadCount + 256 / threadCount;
            threadParams[i].type = type;
            pthread_create(&threads[i], NULL, findPartialKey,
                           (void *)&threadParams[i]);
        }

        done = false;
        for (int i = 0; i < threadCount; i++) {
            pthread_join(threads[i], NULL);
        }
    }

    combinePartialKeys(Key, pKeys);
    printAsHex("key", Key);

    return 0;
}
