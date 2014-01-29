#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

char LIVE = '#';
char DEAD = ' ';

int MAXX, MAXY;

bool wrap = true, paused = false;

typedef struct {
    int x;
    int y;
} point;

char **map;

//wraps around
char getCharAt(int y, int x) {
    if (wrap) {
        if (y < 0) {
            y = MAXY+y;
        }
        if (x < 0) {
            x = MAXX+x;
        }
        if (y >= MAXY) {
            y = 0+(y%MAXY);
        }
        if (x >= MAXX) {
            x = 0+(x%MAXX);
        }
    }
    else if (y < 0 || x < 0 || y >= MAXY || x >= MAXX) {
        return DEAD;
    }
    return map[y][x];
}

int neighbourCount(int y, int x) {
    int neighbours = 0;
    for (int yi = -1; yi < 2; yi++) {
        for (int xi = -1; xi < 2; xi++) {
            if (xi != 0 || yi != 0) {
                neighbours += (getCharAt(y+yi, x+xi) == LIVE) ? 1 : 0;
            }
        }
    }
    return neighbours;
}

void updateMap() {
    char **newmap = malloc(MAXY*sizeof(char*));
    for (int y = 0; y < MAXY; y++) {
        newmap[y] = malloc(MAXX*sizeof(char));
        for (int x = 0; x < MAXX; x++) {
            int neighbours = neighbourCount(y, x);
            if (map[y][x] == LIVE) {
                if (neighbours < 2) {
                    newmap[y][x] = DEAD;
                }
                else if (neighbours == 2 || neighbours == 3) {
                    newmap[y][x] = LIVE;
                }
                else {
                    newmap[y][x] = DEAD;
                }
            }
            else {
                if (neighbours == 3) {
                    newmap[y][x] = LIVE;
                }
                else {
                    newmap[y][x] = DEAD;
                }
            }
        }
    }

    for (int y = 0; y < MAXY; y++) {
        free(map[y]);
    }
    free(map);

    map = newmap;
    return;
}

void drawMap() {
    for (int y = 0; y < MAXY; y++) {
        move(y, 0);
        for (int x = 0; x < MAXX; x++) {
            addch(map[y][x]);
        }
    }
}

int main(int argc, char *argv[]) {

    int c, seed = time(NULL), delay = 1000000/20;
    point cursor;

    while ((c = getopt(argc, argv, "whs:l:r:")) != -1) {
        switch (c) {
            case 'w':
                wrap = false;
                break;
            case 'h':
            case '?':
                printf("%s options:\n  -w Turn off edge wrapping\n  -h Show this message\n  -s [seed] Seed randomization with a given number\n  -l [char] Represent live cells with the given character\n  -r [rate] Update the game [rate] times per second (0 for no delay)\n", argv[0]);
                return 0;
                break;
            case 's':
                seed = atoi(optarg);
                break;
            case 'l':
                LIVE = optarg[0];
                break;
            case 'r':
                delay = (atoi(optarg) == 0) ? 0 : 1000000/atoi(optarg);
                break;
        }

    }

    initscr();
    raw();
    noecho();
    getmaxyx(stdscr, MAXY, MAXX);
    nodelay(stdscr, true);

    srand(seed);
    
    cursor.x = MAXX/2; 
    cursor.y = MAXY/2;
    
    map = malloc(MAXY*sizeof(char*));
    for (int i = 0; i < MAXY; i++) {
        map[i] = malloc(MAXX*sizeof(char));
        for (int j = 0; j < MAXX; j++) {
            if (i >= MAXY/4 && i <= 3*(MAXY/4) && j >= MAXX/4 && j <= 3*(MAXX/4) && rand()%4 == 0) {
                map[i][j] = LIVE;
            }
            else {
                map[i][j] = DEAD;
            }
        }
    }

    while ((c=getch()) != 'q') {
        flushinp();
        switch(c) {
            case 'p':
                paused = (paused) ? false : true; break;
            case 't':
            case ' ':
                map[cursor.y][cursor.x] = (map[cursor.y][cursor.x] == LIVE) ? DEAD :  LIVE;
                break;
            case 'h':
                cursor.x -= (cursor.x == 0) ? 0 : 1;
                break;
            case 'j':
                cursor.y += (cursor.y == MAXY-1) ? 0 : 1;
                break;
            case 'k':
                cursor.y -= (cursor.y == 0) ? 0 : 1;
                break;
            case 'l':
                cursor.x += (cursor.x == MAXX-1) ? 0 : 1;
                break;
            case 'c':
                for (int y = 0; y < MAXY; y++) {
                    for (int x = 0; x < MAXX; x++) {
                        map[y][x] = DEAD;
                    }
                }
                break;
            case 'r':
                for (int i = 0; i < MAXY; i++) {
                    for (int j = 0; j < MAXX; j++) {
                        if (i >= MAXY/4 && i <= 3*(MAXY/4) && j >= MAXX/4 && j <= 3*(MAXX/4) && rand()%4 == 0) {
                            map[i][j] = LIVE;
                        }
                        else {
                            map[i][j] = DEAD;
                        }
                    }
                }
                break;
        }
        drawMap();
        if (!paused) {
            usleep(delay);
            updateMap();
        }
        else {
            move(cursor.y, cursor.x);
        }
    }

    endwin();

    for (int y = 0; y < MAXY; y++) {
        free(map[y]);
    }
    free(map);
    return 0;
}
