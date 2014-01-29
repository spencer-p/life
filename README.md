life
====

a simple command line implementation of conway's game of life with ncurses

some command line options:

    -w Turn off edge wrapping
    -h Show this help message
    -s [seed] Seed randomization with given number
    -l [char] Represent live cells with the given character
    -r [rate] Update the game [rate] times per second (0 for no delay)

some commands inside the program:

    p - Pause/play the "game"
    t/Space - toggle the state of the cell under the cursor
    h/j/k/l - move cursor vi - style
    c - clear the board
    r - randomize the board