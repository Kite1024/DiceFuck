#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>

typedef unsigned char ubyte;

// TODO: Add CLI arg
int verbosity = 0;

#define DEBUG(v, ...) \
    if (verbosity >= (v)) { \
        fprintf(stderr, __VA_ARGS__); \
    }

static ubyte byte_mask[] = {
    0b00000000,
    0b00000001,
    0b00000011,
    0b00000111,
    0b00001111,
    0b00011111,
    0b00111111,
    0b01111111,
    0b11111111
};

// Structure of a cell
struct cell;
typedef struct cell {
    ubyte* value;
    struct cell* prev;
    struct cell* next; 
} cell;

// Number of bits and ceil(number of bytes) of a cell value
int n_bits = 0;
int n_bytes = 0;

// Current cell and index
int cellidx = 0;
cell* cur;

// Program, position within it, and size
char* prog;
int pos = 0;
long progsize = 0;

// List of executed (face,value) instruction pairs.
// And instruction pointer (ip) pointing to the current instruction index
ubyte ins[30000];
int inssize = 0;
int ip = 0;

// Output buffer and size
ubyte outbuf = 0;
int outbuf_bits = 0;

// Input buffer and size
ubyte inbuf = 0;
int inbuf_bits = 0;

// Reading direction and stack, used for loops
int dir = 1; // 1=forward, -1=backward
int stack = 0; // nesting level

void error(char* str) {
    fprintf(
        stderr,
        "Error: %s\n"
        "\tat dice(%d, %d)\n"
        "\tcell %d (%d)\n"
        "\tip %d\n",
        str,
        ins[ip], ins[ip+1],
        cellidx, cur->value[0],
        ip);
    exit(1);
}

cell* makecell(cell* prev, cell* next) {
    cell* n = malloc(sizeof(cell));

    n->value = malloc(n_bytes);
    memset(n->value, 0, n_bytes);

    n->prev = prev;
    n->next = next;
    return n;
}

void nextcell() {
    cell* n = cur->next;
    if (n == NULL) {
        n = makecell(cur, NULL);
        cur->next = n;
    }

    cellidx++;
    cur = n;
}

void prevcell() {
    cell* n = cur->prev;
    if (n == NULL) {
        n = makecell(NULL, cur);
        cur->prev = n;
    }

    cellidx--;
    cur = n;
}

void addval(int n) {
    //fprintf(stderr, "Add(%d): %d\n", n, cur->value[0]);
    int carry = n;
    for (int i = 0; i < n_bytes; i++) {
        carry += cur->value[i];
        cur->value[i] = (ubyte) carry;
        carry >>= 8;
    }
    DEBUG(1, "Add %d to c%d => %d\n", n, cellidx, cur->value[0])

    // TODO: Maybe wrap around to higher values go back to zero
    if (carry != 0) {
        error("Value byte over/underflow at cell");
    }

    int lastbits = n_bits % 8;
    //fprintf(stderr, "Lastbits: %d\n", lastbits);
    if (lastbits != 0 && (cur->value[n_bytes-1] >> lastbits) > 0) {
        error("value bit overflow at cell");
    }
}

void subval(int n) {
    // Adapted from
    // https://stackoverflow.com/questions/61488609/substract-an-char-array-minus-int-in-c
    
    // Can we avoid borrowing and do a local subtract?
    if (n <= cur->value[0]) {
        cur->value[0] -= n;
        DEBUG(1, "Sub %d from c%d => %d\n", n, cellidx, cur->value[0])
        return;
    }

    // Try borrowing from subsequent bytes
    for (int i = 1; i < n_bytes; i--){
        // Can we borrow?
        if (cur->value[i] == 0){
            continue;
        }

        // Borrow
        cur->value[i]--;
        for (int j = i - 1; j > 0; j--){
            cur->value[j] = 255;
        }

        int newval = ((int)(cur->value[0]) + 255 - n);
        cur->value[0] = (ubyte) newval;
        return;
    }

    error("Value byte underflow at cell");
}

void outval() {
    // Number of bits we _want_ to output
    int b = n_bits + outbuf_bits;
    int i = 0;

    while (b > 0) {
        ubyte d = cur->value[i];

        if ((outbuf_bits + b) < 8) {
            // Not enough bits (left) to output
            // Shift bits into outbuf
            // Also handles b=0
            outbuf <<= b;
            outbuf |= d & byte_mask[b];
            outbuf_bits += b;
            return;
        }

        // We have enough bits to output, 
        int new_bits = 8 - outbuf_bits;
        char out = 0;
        out |= outbuf_bits << new_bits;
        out |= d & byte_mask[new_bits];
        putchar(out);
        b -= 8;

        // Update the buffer
        int unused_bits = 8 - new_bits; // # of bits of this byte that's unused
        int unused_mask = byte_mask[unused_bits] << new_bits;
        outbuf = (d & unused_mask) >> new_bits; // Store the bits shifted all the way right
        outbuf_bits = unused_bits;

        i += 1; // Next byte
    }
}

void inval() {
    // Clear the current value
    memset(cur->value, 0, n_bytes);

    // Number of bytes we need to input
    int bytes_in = ((n_bits - inbuf_bits - 1) / 8) + 1;
    
    char in[bytes_in];
    for (int i = 0; i < bytes_in; i++) {
        in[i] = getchar();
    }

    int b_left = n_bits;
    int i = 0;
    while (b_left > 0) {
        
        ubyte d = in[i];

        int new_bits = 8 - inbuf_bits;

        ubyte in = 0;
        in |= (inbuf << new_bits);
        in |= (d & byte_mask[new_bits]);
        cur->value[i] = in;

        // Shift the other part of d into the buffer
        int unused_bits = 8 - new_bits;
        int unused_mask = byte_mask[unused_bits] << new_bits;
        inbuf = (d & unused_mask) >> new_bits;
        inbuf_bits = unused_bits;

        b_left -= 8;
        i += 1;
    }

}

void dice(int sides, int value) {
    DEBUG(2, "ip %d dir %d: Dice %d=%d\n", ip, dir, sides, value)

    switch (sides) {
    case 4:
        if (stack > 0) return;

        if (value == 1) {
            prevcell();
            prevcell();
        } else if (value == 2) {
            prevcell();
        } else if (value == 3) {
            nextcell();
        } else if (value == 4) {
            nextcell();
            nextcell();
        } else {
            error("Out of range d4");
        }
        break;

    case 12:
        if (stack > 0) return;

        if (value >= 1 && value <= 6) {
            subval(7 - value);
        } else if (value >= 7 && value <= 12) {
            addval(value - 6);
        } else {
            error("Out of range d12");
        }
        break;
    
    case 6:
        if (stack > 0) return;

        if (value != 6) {
            error("Cannot have d6 with a value except 6");
        }

        outval();
        break;

    case 8:
        if (stack > 0) return;

        if (value != 8) {
            error("Cannot have d8 with a value except 8");
        }
        inval();
        break;

    case 10:
        if (value != 10) {
            error("Cannot have d10 with a value except 10");
        }

        // Termination condition
        if (stack == 1 && dir == -1) {
            // We're seeking back from a ']', so reset
            dir = 1;
            stack = 0;
            return;
        } else if (stack > 0) {
            stack += dir; // Already in a loop, but seeking
            return;
        }

        // Start a '[' loop
        for (int i = 0; i < n_bytes; i++) {
            if (cur->value[i] != 0) {
                return; // not zero, do nothing
            }
        }

        // Seek to the matching ']'
        dir = 1;
        stack = 1;

        break;

    case 100:
        if (value != 100) {
            error("Cannot have d100 with a value except 100");
        }

        // Termination instruction
        if (stack == 1 && dir == 1) {
            // We're seeking forward from a matching '[', reset
            dir = 1;
            stack = 0;
            return;
        } else if (stack > 0) {
            stack -= dir; // Already in a loop, but seeking
            return;
        }

        // stack==0, backward seeking
        for (int i = 0; i < n_bytes; i++) {
            if (cur->value[i] == 0) {
                continue;
            }

            // Non-zero, seek back
            dir = -1;
            stack = 1;
        }

        // Zero, do nothing
        break;

    case 20:
        if (ip != 0) {
            error("d20 used as non-start instruction");
        }

        n_bits = 1 << (value-1);
        n_bytes = ((n_bits - 1) / 8) + 1;
        DEBUG(1, "Cell bits: %d, bytes: %d\n", n_bits, n_bytes)
        
        // Make the first cell
        cur = makecell(NULL, NULL);
        break;
    
    default:
        error("Unknown number of sides");
    }
}

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

bool next_instr() {
    ip += (dir * 2);

    if (ip >= inssize) {
        if (stack > 0) {
            error("Unmatched d10");
        }

        return false; // Program finished
    }

    if (ip < 0) {
        error("Unmatched d100");
    }

    dice(ins[ip], ins[ip+1]);
    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        exit(1);
    }

    // Read
    readfile(argv[1]);

    // Parse
    parseprog();
    free(prog);

    // Execute
    dir = 1;
    ip = -2;
    while (next_instr()) ;
    //printf("\n");

    //fprintf(stderr, "Done!\n");

    // TODO: flush buffer?
    exit(0);
}
