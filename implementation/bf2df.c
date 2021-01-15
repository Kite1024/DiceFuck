#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COMTAB 5

int ind = 0;

char* comment;

void make_comment(char c, int r) {
    for (int i = 0; i < r; i++) {
        comment[i] = c;
    }
    comment[r] = '\0';
}


#define OUT(fmt, ...) \
    for (int i = 0; i < ind; i++) {\
        fprintf(out, "\t");\
    }\
    fprintf(out, fmt, ##__VA_ARGS__);\
    for (int i = 0; i < COMTAB - ind; i++) {\
        fprintf(out, "\t");\
    }\
    fprintf(out, "%s\n", comment);

#define NL()\
    fprintf(out, "\n");

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <in> <out>\n", argv[0]);
        exit(1);
    }

    // Read input into str
    FILE* in = fopen(argv[1], "rb");
    if (!in) {
        perror(argv[1]);
        exit(1);
    }

    // Find in length
    fseek(in, 0L, SEEK_END);
    long progsize = ftell(in);
    rewind(in);

    char* prog = calloc(1, progsize+1);
    if (!prog) {
        fclose(in);
        fputs("prog alloc fail\n", stderr);
        exit(1);
    }

    if (1 != fread(prog, progsize, 1, in)) {
        fclose(in);
        free(prog);
        fprintf(stderr, "prog read fail: %ld\n", progsize);
        exit(1);
    }
    fclose(in);

    // Write output
    FILE* out = fopen(argv[2], "wb");
    if (!out) {
        perror(argv[2]);
        exit(1);
    }

    fprintf(out, "Input: %s\n\n", argv[1]);
    int i = -1;

    comment = malloc(512);
    strcpy(comment, "Cell size: 8 bits\n");
    OUT("d20=4\t")
    while (i < progsize) {
        i++;
        char c = prog[i];

        int r = 1; // repetitions
        while (progsize - i >= 1 && prog[i+1] == c) {
            r++;
            i++;
        }

        switch (prog[i]) {
            case '<':
                while (r >= 2) {
                    make_comment(c, 2);
                    OUT("d4=1\t")
                    r -= 2;
                }
                while(r > 0) {
                    make_comment(c, 1);
                    OUT("d4=2\t")
                    r--;
                }
                break;

            case '>':
                while (r >= 2) {
                    make_comment(c, 2);
                    OUT("d4\t\t")
                    r -= 2;
                }
                while(r > 0) {
                    make_comment(c, 1);
                    OUT("d4=3\t")
                    r--;
                }
                break;

            case '+':
                while (r >= 6) {
                    make_comment(c, 6);
                    OUT("d12\t\t")
                    r -= 6;
                }
                if (r > 0) {
                    make_comment(c, r);
                    OUT("d12=%d\t", r + 6)
                }
                break;

            case '-':
                while (r >= 6) {
                    make_comment(c, 6);
                    OUT("d12=1\t")
                    r -= 6;
                }
                if (r > 0) {
                    make_comment(c, r);
                    OUT("d12=%d\t", 7 - r)
                }
                break;

            case '.':
                while (r-- > 0) {
                    make_comment(c, 1);
                    OUT("d6\t\t")
                }
                break;

            case ',':
                while (r-- > 0) {
                    make_comment(c, 1);
                    OUT("d8\t\t")
                }
                break;

            case '[':
                while (r-- > 0) {
                    NL()
                    make_comment(c, 1);
                    OUT("d10\t\t")
                    ind++;
                }
                break;

            case ']':
                while (r-- > 0) {
                    ind--;
                    make_comment(c, 1);
                    OUT("d100\t")
                    NL()
                }
                break;
            
        }

    }

    fclose(out);
}
