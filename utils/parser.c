#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void parse(unsigned char *dst, char *str) {
    str[strcspn(str, "\n")] = 0;
    unsigned char *ptr = dst;
    for (; *str; ptr++) {
        while (isspace(*str))
            str++;
        *ptr = strtoul(str, (char **)&str, 16);
    }
}

void readfile(const char *filename, unsigned char *pt1, unsigned char *pt2) {
    FILE *file = fopen(filename, "r");
    char line[64];

    fgets(line, sizeof(line), file);
    parse(pt1, line);

    fgets(line, sizeof(line), file);
    parse(pt2, line);

    fclose(file);
}