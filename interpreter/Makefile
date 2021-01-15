default: compile

.PHONY: compile test seven
compile: dice

CC = gcc
CSTD = -std=c99
COPTS = -Wall -Wextra -Werror -Wno-unused-parameter

dice: dice.c
	$(CC) $(COPTS) -o dice dice.c

test: dice
	dice hello.df

seven: dice
	dice seven.df
