//
// Created by konrad on 09.01.2022.
//

#include "NcursesUtil.h"
#include <ncurses.h>
#include <cstdlib>

void ncurses_util::Initialize() {
    initscr();
    cbreak();
    noecho();
    clear();
    refresh();
    curs_set(0);

    if(!has_colors())
    {
        endwin();
        printf("No color support");
        exit(1);
    }

    start_color();
}

short ncurses_util::CursColor(int fg) {
    switch (7 & fg) {           /* RGB */
        case 0:                     /* 000 */
            return (COLOR_BLACK);
        case 1:                     /* 001 */
            return (COLOR_RED);
        case 2:                     /* 010 */
            return (COLOR_GREEN);
        case 3:                     /* 011 */
            return (COLOR_YELLOW);
        case 4:                     /* 100 */
            return (COLOR_BLUE);
        case 5:                     /* 101 */
            return (COLOR_MAGENTA);
        case 6:                     /* 110 */
            return (COLOR_CYAN);
        case 7:                     /* 111 */
            return (COLOR_WHITE);
        default:
            return (COLOR_BLACK);
    }
}

int ncurses_util::ColorNum(int fg, int bg) {
    int B, bbb, ffff;

    B = 1 << 7;
    bbb = (7 & bg) << 4;
    ffff = 7 & fg;

    return (B | bbb | ffff);
}

void ncurses_util::InitAllPossibleColorPairs() {
    int fg, bg;
    int color_pair;

    for (bg = 0; bg <= 7; bg++) {
        for (fg = 0; fg <= 7; fg++) {
            color_pair = ncurses_util::ColorNum(fg, bg);
            init_pair(color_pair, ncurses_util::CursColor(fg), ncurses_util::CursColor(bg));
        }
    }
}
