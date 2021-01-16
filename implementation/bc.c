#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "bc.h"

// TODO: Add CLI arg
int verbosity = 0;

// List of executed (face,value) instruction pairs.
// And instruction pointer (ip) pointing to the current instruction index
ubyte* ins;
static int inssize = 0;

char* prog;
long progsize;
int pos;

void readfile(char* file) {

    FILE* fp = fopen(file, "rb");
    if (!fp) {
        perror(file);
        exit(1);
    }

    // Find program length
    fseek(fp, 0L, SEEK_END);
    progsize = ftell(fp);
    rewind(fp);

    prog = malloc(progsize+1);
    if (!prog) {
        fclose(fp);
        fputs("prog alloc fail\n", stderr);
        exit(1);
    }

    if (1 != fread(prog, progsize, 1, fp)) {
        fclose(fp);
        free(prog);
        fprintf(stderr, "prog read fail: %ld\n", progsize);
        exit(1);
    }

    fclose(fp);
}

int matchint(char* prog, int p, int* val) {
    if (!isdigit(prog[p])) {
        return -1;
    }

    // See if there's:
    // * no second digit (d6)
    // * a second digit (d20)
    // * a third digit (d100)
    int v;
    int len = 0;
    if (!isdigit(prog[p+1])) {
        v = (prog[p] - '0');
        len = 1;
    } else if (isdigit(prog[p+1]) && !isdigit(prog[p+2])) {
        v = (prog[p] - '0') * 10;
        v += (prog[p+1] - '0');
        len = 2;
    } else if (isdigit(prog[p+1]) && isdigit(prog[p+2])) {
        v = (prog[p] - '0') * 100;
        v += (prog[p+1] - '0') * 10;
        v += (prog[p+2] - '0');
        len = 3;
    }

    *val = v;
    return len;
}

void parseprog() {
    pos = -1;
    while (pos < progsize) {
        pos++; // next char
        if (progsize - pos < 3) continue;

        // match dX
        if (prog[pos] != 'd') continue;

        pos++;
        int sides = 0;
        int skip = matchint(prog, pos, &sides);
        if (skip < 1) { continue; }
        pos += skip;

        // match '=X'
        int value = sides;
        if (prog[pos] == '=' && (progsize - pos) >= 3) {
            pos++;
            skip = matchint(prog, pos, &value);
            if (value < 0) { // Handle d5=Bla
                value = sides;
            } else {
                pos += skip;
            }
        }

        // Push to executions
        if (inssize == 0 && sides != 20) { // Initial d20
            DEBUG(2, "Insert initial die (%d): d%d=%d\n", inssize, sides, value)
            ins[inssize++] = 20;
            ins[inssize++] = 4; // 2^(4-3) = 8 bit cells
        }
        
        DEBUG(2, "Die (%d): d%d=%d\n", inssize, sides, value)
        ins[inssize++] = sides;
        ins[inssize++] = value;
    }
}

int bc_read(char* file, ubyte* target, int maxins) {
    ins = target;

    readfile(file);
    parseprog();
    free(prog);

    return inssize; // TODO: maxins
}
