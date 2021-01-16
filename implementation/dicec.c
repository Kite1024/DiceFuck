#include <stdio.h>
#include <stdlib.h>

#include "bc.h"

#define OUT(fmt, ...) printf(fmt "\n", ##__VA_ARGS__);

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: %s <input> <output>\n", argv[0]);
        exit(1);
    }

    ubyte* ins = malloc(600000);
    int inssize = bc_read(argv[1], ins, 600000);
    
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

    return 0;
}
