#include "../utils/constants.h"
#include "../utils/debug.h"
#include "../utils/transformation.h"
#include "../utils/parser.h"
#include <stdio.h>
#include <string.h>

#define NUM 10

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
    /* clang-format off */
    char *raw_plain_texts[NUM] = {
        "9fb1c5a70f62b3cf4f916aa90466d2bd",
        "9a23aaa7624a63b8c59889e9c1367ac3",
        "4d27dd59fefb5c393fbfcd122eea2cbe",
        "4f521cf982a990f104cebbb6378774ec",
        "2d083a63c393a2b15d805ddd5e905143",
        "7449b19db5db5888db1cd18a4e3473e1",
        "cb2260d2f4cf96f9d4586b7d32d6889f",
        "ffdfe7e8af48382dd56e59dd7c832f8f",
        "f3a1f9a5f72648a09b48effe6713996c",
        "3c9e103bda66599e61bbc845b2ee900e",
    };
    char *raw_keys[NUM] = {
        "f47e7ce7426e99a526f58a803d968700",
        "c3b7be977353d7bff0387812e3cc93a6",
        "744b40b8412e49d856745ec5003c5a08",
        "fd4e12ea5c8c2bb3f9494928d3492c59",
        "6e396fd4c79f23c565f8b50331a94584",
        "f3b70aca8da04355c685242b1f132b8a",
        "7b496f20c948bbfcf7c9fca1dcfc7ecd",
        "a20e2455ceea6229866497f88e191b14",
        "9f9a69db9cd2e2e0694700b2d0d48123",
        "d5ec10700730e2ba0e0549fd0cf26cbf",
    };
    char *expected_raw_cipher_texts[NUM] = {
        "9a23aaa7624a63b8c59889e9c1367ac3",
        "683aacf285af7b80c0b5b9ff088d1e4b",
        "e77bac30dae58e1d1b9a109be155b302",
        "efe157eff4878acc1670f7fcfd10e635",
        "e7c8c0b20c4bed34b36040f2c40124b7",
        "ded81dad29e35c135fec4848d41418c4",
        "23a77183b9749feebd827d0b8256cb84",
        "f3b330fdb2bd18648abc67a8123e84ba",
        "d257f4211213b505a581224d2a7548ec",
        "ed3e2284a5a468cea8ca1a468c949cfb",
    };
    /* clang-format on */

    for (int i = 0; i < NUM; i++) {
        unsigned char plain_text[16], key[16], cipher_text[16], expected_cipher_text[16];

        parseHexString(expected_cipher_text, expected_raw_cipher_texts[i]);
        parseHexString(plain_text, raw_plain_texts[i]);
        parseHexString(key, raw_keys[i]);

        Encrypt(cipher_text, plain_text, key);

        printf("======== TEST %d ========\n", i);
        printAsHex("plain_text          ", plain_text);
        printAsHex("key                 ", key);
        printAsHex("cipher_text         ", cipher_text);
        printAsHex("expected_cipher_text", expected_cipher_text);
    }

    return 0;
}
