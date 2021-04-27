.PHONY: compile test seven test-dice-small test-transpile

default: all

all: dice bf2df

CC = gcc
CSTD = -std=c99
COPTS = -Wall -Wextra -Werror -Wno-unused-parameter
COPTS_SMALL = $(COPTS) \
	-s\
	-Os\
	-m32\
	-mpreferred-stack-boundary=2\
	-falign-functions=1\
	-falign-jumps=1\
	-falign-loops=1\
	-ffunction-sections\
	-fdata-sections\
	-fno-stack-protector\
	-fomit-frame-pointer\
	-fno-unwind-tables\
	-fno-math-errno\
	-fno-unroll-loops\
	-fmerge-all-constants\
	-fno-ident\
	-fno-asynchronous-unwind-tables\
	-Wl,--gc-sections\
	-Wl,--build-id=none

STRIP_OPT =\
	-S\
	--strip-unneeded\
	--remove-section=.note.gnu.gold-version\
	--remove-section=.comment\
	--remove-section=.note\
	--remove-section=.note.gnu.build-id\
	--remove-section=.note.ABI-tag\
	-s\
	-R .comment\
	-R .gnu.version

dice: dice.c
	$(CC) $(COPTS) -o dice dice.c

dice-small: dice.c
	$(CC) $(COPTS_SMALL) -o dice-small dice.c
	strip $(STRIP_OPT) dice-small.exe

test-dice-small: dice-small
	dice-small ../examples/hello.df

test: dice
	dice ../examples/hello.df

seven: dice
	dice ../examples/seven.df

bf2df: bf2df.c
	$(CC) $(COPTS) -o bf2df bf2df.c

test-transpile: bf2df dice
	bf2df bf/hello.bf bf/hello.df
	dice bf/hello.df
	bf2df bf/fact.bf bf/fact.df
	dice bf/fact.df
# TODO: Broken :(
#	bf2df bf/bottles.bf bf/bottles.df
#	dice bf/bottles.df
#	bf2df bf/mandelbrot.bf bf/mandelbrot.df
#	dice bf/mandelbrot.df
