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
    char **files;
    int type;
    int num;
    int from;
    int to;
} arg_struct;

bool done;
const int threadCount = 8;

void *findPartialKey(void *threadArg) {
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

                    int i;
                    for (i = 0; i < arg->num; i++) {
                        memcpy(state1, pt1[i], sizeof(state1));
                        memcpy(state2, pt2[i], sizeof(state2));

                        Transform(state1, key);
                        Transform(state2, key);

                        if (!isOneByteDiff(state1, state2, arg->type)) {
                            break;
                        }
                    }
                    if (i == arg->num) {
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
}

int main() {
    unsigned char pKeys[4][16];
    unsigned char Key[16];

    /* clang-format off */
    char *files[4][10] = {{"pt/8", "pt/9", "pt/12", "pt/15"},
                          {"pt/10", "pt/14", "pt/20"},
                          {"pt/1", "pt/2", "pt/3", "pt/11", "pt/13", "pt/17", "pt/18"},
                          {"pt/4", "pt/5", "pt/6", "pt/7", "pt/16", "pt/19"}};
    const int fileCounts[4] = {4, 3, 7, 6};
    /* clang-format on */

    for (int t = 0; t < 4; t++) {
        pthread_t threads[threadCount];
        arg_struct threadParams[threadCount];

        for (int i = 0; i < threadCount; i++) {
            threadParams[i].dst = pKeys[t];
            threadParams[i].files = files[t];
            threadParams[i].num = fileCounts[t];
            threadParams[i].from = i * 256 / threadCount;
            threadParams[i].to = i * 256 / threadCount + 256 / threadCount;
            threadParams[i].type = t;
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
