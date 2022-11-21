#include <stdio.h>

void print16bytes(char *title, unsigned char *src){
    printf("%s: 0x", title);
    for(int i = 0; i < 16; i++){
        printf("%02x", src[i]);
    }
    printf("\n");
}