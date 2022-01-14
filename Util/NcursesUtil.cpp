//
// Created by konrad on 09.01.2022.
//

#include "NcursesUtil.h"
#include <ncurses.h>
#include <cstdlib>
#include <algorithm>

using namespace std;

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

short ncurses_util::CursColor(int color) {
    return (short)(15 & color);
}

int ncurses_util::ColorNum(int fg, int bg) {
    int bbbb, ffff;

    bbbb = (15 & bg) << 4;
    ffff = (15 & fg);

    return (bbbb | ffff);
}

void ncurses_util::InitAllPossibleColorPairs() {
    int fg, bg;
    int color_pair;

    for (bg = 0; bg < 16; bg++) {
        for (fg = 0; fg < 16; fg++) {
            color_pair = ncurses_util::ColorNum(fg, bg);
            init_pair(color_pair, ncurses_util::CursColor(fg), ncurses_util::CursColor(bg));
        }
    }
}

void ncurses_util::SetColor(int fg, int bg) {
    attron(COLOR_PAIR(ncurses_util::ColorNum(fg, bg)));
}

void ncurses_util::UnsetColor(int fg, int bg) {
    attroff(COLOR_PAIR(ncurses_util::ColorNum(fg, bg)));
}

void ncurses_util::AddText(Vec2i screen_coords, std::string text, int fg_color, int bg_color) {
    SetColor(fg_color, bg_color);
    mvaddstr(screen_coords.y, screen_coords.x, text.c_str());
    UnsetColor(fg_color, bg_color);
}

void ncurses_util::AddTextColorPair(Vec2i screen_coords, std::string text, int color_pair) {
    attron(COLOR_PAIR(color_pair));
    mvaddstr(screen_coords.y, screen_coords.x, text.c_str());
    attroff(COLOR_PAIR(color_pair));
}

void ncurses_util::AddText(Vec2i screen_coords, char character, int fg_color, int bg_color) {
    AddText(screen_coords, string(1, character), fg_color, bg_color);
}

void ncurses_util::AddTextColorPair(Vec2i screen_coords, char character, int color_pair) {
    AddTextColorPair(screen_coords, string(1, character), color_pair);
}

void ncurses_util::ConsoleWriter::ModifyIndent(int size) {
    indent = max(0, indent + size);
}

void ncurses_util::ConsoleWriter::AddLine(std::string text, int fg_color, int bg_color) {
    AddText(console_screen_pos + Vec2i(indent, line), text, fg_color, bg_color);
    line++;
}
