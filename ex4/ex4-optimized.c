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

    for (int k0 = 0; k0 < 256; k0++) {
          
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
