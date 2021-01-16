#ifndef BC_H
#define BC_H

extern int verbosity;

#define DEBUG(v, ...) \
    if (verbosity >= (v)) { \
        fprintf(stderr, __VA_ARGS__); \
    }

// Unsigned byte
typedef unsigned char ubyte;

int bc_read(char* file, ubyte* ins, int maxins);

#endif
