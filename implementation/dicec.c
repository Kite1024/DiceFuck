#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bc.h"

#define OUT(fmt, ...) fprintf(fp, fmt "\n", ##__VA_ARGS__);

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: %s [-s] <input> <output>\n", argv[0]);
        exit(1);
    }

    int source = 0;
    if (strcmp(argv[1], "-s") == 0) {
        if (argc < 4) {
            printf("Usage: %s [-s] <input> <output>\n", argv[0]);
            printf("\t-s\t\tOutput instead the C source");
            exit(1);
        }

        source = 1;

        argv[1] = argv[2];
        argv[2] = argv[3];
    }

    ubyte* ins = malloc(600000);
    int inssize = bc_read(argv[1], ins, 600000);
    
    char outtemp[512];
    strcpy(outtemp, argv[2]);
    if (!source) {
        strcat(outtemp, ".c");
    }
    FILE* fp = fopen(outtemp, "wb");

    OUT("#include <stdio.h>")
    OUT("")
    OUT("unsigned char a[30000];")
    OUT("int i = 0;")
    OUT("")
    OUT("int main(int argc, char* argv[]) {")

    int i = -2;
    while (i < inssize) {
        i += 2;

        int s = ins[i];
        int v = ins[i+1];
        
        switch (s) {
        case 4:
            if (v == 1 || v == 2) {
                OUT("i -= %d;", 3 - v)
            } else {
                OUT("i += %d;", v - 2)
            }
            break;

        case 12:
            if (v >= 1 && v <= 6) {
                OUT("a[i] -= %d;", 7 - v)
            } else {
                OUT("a[i] += %d;", v - 6)
            }
            break;
            
        case 6:
            OUT("putchar(a[i]);")
            break;

        case 8:
            OUT("a[i] = getchar();")
            break;
        
        case 10:
            OUT("while (a[i]) {")
            break;

        case 100:
            OUT("}")
            break;

        case 20:
            break;
        
        }

    }

    OUT("}")

    fclose(fp);

    if (!source) {
        char cmd[512];
        sprintf(cmd, "gcc -O2 -o %s %s && rm %s", argv[2], outtemp, outtemp);
        system(cmd);
    }

    return 0;
}
