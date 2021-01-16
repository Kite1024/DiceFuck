#include <stdio.h>
#include <stdlib.h>

#include "bc.h"

#define OUT(fmt, ...) printf(fmt "\n", ##__VA_ARGS__);

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: %s <input> <output>\n", argv[0]);
        exit(1);
    }

    ubyte* ins = malloc(30000);
    int inssize = bc_read(argv[1], ins, 30000);
    
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
                OUT("i -= %d", 3 - v)
            } else {
                
            }
            break;

        case 12:
            break;
            
        case 6:
            break;

        case 8:
            break;
        
        case 10:
            break;

        case 100:
            break;

        case 20:
            break;
        
        }

    }

    OUT("}")

    return 0;
}
