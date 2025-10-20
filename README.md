# Author
Glenn Benisek

# Course
Fall Semester 2025
CS 3503: Computer Organization Arch
Instructor: Christopher Regan

# Project
CS 3503 Project 1 - Bitboard

# Build Instructions
```bash
gcc -o checkers checkers.c
./checkers
```
# Description
To move a piece, submit two positions defined by the coordinates seperated with a space. 
The coordinates are invalid if:
1. Not in 'nx nx' format, ie. 'a3 b4'.
2. An opponents piece or an empty square exists at the origin coordinate.
3. The destination is occupied or outside of the gird.
4. One of your pieces can capture an opponent.
5. Not moving in a 1 square diagonal line. (Except for capturing).

Type 'binary' to obtain the board data in binary.
Type 'hex' to obtain the board data in hexadecimal.
Type 'save {name}' or 'load {name}' to save/load a game state.
Type 'quit' to exit.

Note: There exists two uint64_t types (pieces and kings), organized into bits (0-31, 32-63). 
Checkers never plays on white squares, compresseing black and red bit data into a single 64 bit int.
This enables only two 64 bits to be used, versus four when including white squares.

    a   b   c   d   e   f   g   h
  ┏━━━┳━━━┳━━━┳━━━┳━━━┳━━━┳━━━┳━━━┓
8 ┃▓▓▓┃ b ┃▓▓▓┃ b ┃▓▓▓┃ b ┃▓▓▓┃ b ┃ 8
  ┣━━━╋━━━╋━━━╋━━━╋━━━╋━━━╋━━━╋━━━┫
7 ┃ b ┃▓▓▓┃ b ┃▓▓▓┃ b ┃▓▓▓┃ b ┃▓▓▓┃ 7
  ┣━━━╋━━━╋━━━╋━━━╋━━━╋━━━╋━━━╋━━━┫
6 ┃▓▓▓┃ b ┃▓▓▓┃ b ┃▓▓▓┃ b ┃▓▓▓┃ b ┃ 6
  ┣━━━╋━━━╋━━━╋━━━╋━━━╋━━━╋━━━╋━━━┫
5 ┃   ┃▓▓▓┃   ┃▓▓▓┃   ┃▓▓▓┃   ┃▓▓▓┃ 5
  ┣━━━╋━━━╋━━━╋━━━╋━━━╋━━━╋━━━╋━━━┫
4 ┃▓▓▓┃   ┃▓▓▓┃   ┃▓▓▓┃   ┃▓▓▓┃   ┃ 4
  ┣━━━╋━━━╋━━━╋━━━╋━━━╋━━━╋━━━╋━━━┫
3 ┃ r ┃▓▓▓┃ r ┃▓▓▓┃ r ┃▓▓▓┃ r ┃▓▓▓┃ 3
  ┣━━━╋━━━╋━━━╋━━━╋━━━╋━━━╋━━━╋━━━┫
2 ┃▓▓▓┃ r ┃▓▓▓┃ r ┃▓▓▓┃ r ┃▓▓▓┃ r ┃ 2
  ┣━━━╋━━━╋━━━╋━━━╋━━━╋━━━╋━━━╋━━━┫
1 ┃ r ┃▓▓▓┃ r ┃▓▓▓┃ r ┃▓▓▓┃ r ┃▓▓▓┃ 1
  ┗━━━┻━━━┻━━━┻━━━┻━━━┻━━━┻━━━┻━━━┛
    a   b   c   d   e   f   g   h

player (r) (R)    bot (b) (B)    ▓▓▓ white square
