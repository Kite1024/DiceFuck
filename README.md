# DiceFuck
We present dicefuck, a family of dice-based interpretations/children of the brainfuck language. Two variants are proposed here, one with a full set of tabletop-rpg common dice and one using only six-sided dice. We're very open to discussion in the issues :)

## Tabletop-rpg common dice scheme
Dice used: d4, d6, d8, d10, d12, d20, d100 (split in a d10 and a double-digit d10).

This scheme uses a full repetoire of common rpg dice (at least at my table). It adapts to brainfuck in several ways, first of which is that a 'symbol'(read: die) may have it's value used to prevent repetitiveness in your code (less dice for the same program! Yay for _efficiency_!)

Die | Effect
----|--------
d4  | If value: 1, 2; Decrement pointer address by 2, 1 respectively. <br> If value: 3, 4; Increment pointer address by 1, 2 respectively.
d12 | If value: 1-6; Decrement value at pointer address by 6-1 respectively. <br> If value: 7-12; Increment value at pointer address by 1-6 respectively.
d6  | Output the n bits value at the current pointer address.
d8  | Accept the n bits value input and store it at the current pointer address.
d10 | Same as '[' in parent language (from wiki: "if the byte at the data pointer is zero, then instead of moving the instruction pointer forward to the next command, jump it forward to the command after the matching ] command.")
d100| Use only the double digit die, complements the d10 and represents the same as ']' in the parent language (from wiki: "if the byte at the data pointer is nonzero, then instead of moving the instruction pointer forward to the next command, jump it back to the command after the matching [ command.")
d20 | Optional but if used must be declared at the start of the program (as the first die). It's value represents how many bits are used for the address value size 'n'. The value of n is set to 2^(value-1) bits. If the d20 is not used, n is set to 8 bits (one byte).

Conventions: Whenever a dice has no value worth, it is placed on it's highest value (i.e. d8 with value 8 up, d6 with value 6, etc.). The exclusion to this is whenever a dice is indexed at 0 (i.e. a d100 double-digit die may start/end with 00, in this case the 00 is the value pointed up).

Advantages:
- Allows up to 524.288 bits to be stored behind a single address pointer.
- Finally a new and practical reason to get that extra dice set.
- New options for art, glue your program on something (ask first if the thing you glue it on isn't yours)
- Embrace your inner board-game nerd.

Disadvantages:
- The dice could have allowed for more efficient usage of their states, as only a select number of dice use their values.

## Six-sided die scheme
Dice used: d6

Not everyone enjoys tabletop-rpg's (if that's because you haven't tried them, that might be the issue), and some just enjoy the casual six-sided dies used by pretty much every other game (including some one-page-rpg's, looking at you lasers&feelings). For those people, and the people who prefer their programs to fit nicely side-to-side, there's this scheme using only d6 dice. Because the parent language has 8 symbols and the d6 can only support 6, two of the symbols used will require a follow-up die, as described in their effect.

Die value | Effect
----------|--------
1   | Requires follow up die. <br> Follow-up value: 1-3; Decrement the pointer address by 3-1 respectively. <br> Follow-up value: 4-6; Increment the pointer address by 1-3 respectively.
2   | Requires follow up die. <br> Follow-up value: 1-3; Decrement the value at the current pointer address by 3-1 respectively. <br> Follow-up value: 4-6; Increment the value at the current pointer address by 1-3 respectively.
3   | Output the byte value at the current pointer address.
4   | Accept a byte value of input at the current pointer address.
5   | Same as '[' in parent language (from wiki: "if the byte at the data pointer is zero, then instead of moving the instruction pointer forward to the next command, jump it forward to the command after the matching ] command.")
6   | same as ']' in the parent language (from wiki: "if the byte at the data pointer is nonzero, then instead of moving the instruction pointer forward to the next command, jump it back to the command after the matching [ command.")

Advantages:
- Only uses one type of die, and one that is more easily acquired in bulk.
- If the same die type is used, the program end up in an orderly side-to-side sequence which means cleaner art for those who crave it.
- Due to it's simple layout, one without dice could do with the numbers 1-6 for on the back of an envelope.

Disadvantages:
- Requires a follow-up die for some symbols, increasing complexity
- Probably even harder to read than brainfuck.