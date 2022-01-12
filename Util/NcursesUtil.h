//
// Created by konrad on 09.01.2022.
//

#ifndef PIRATESSIMULATION_NCURSESUTIL_H
#define PIRATESSIMULATION_NCURSESUTIL_H

#include <string>
#include "Vec2i.h"
#include <ncurses.h>

#define COLOR_BROWN 8
#define COLOR_GRAY 9

namespace ncurses_util {
    void Initialize();
    short CursColor(int fg);
    int ColorNum(int fg, int bg);
    void InitAllPossibleColorPairs();
    void SetColor(int fg, int bg);
    void UnsetColor(int fg, int bg);

    class ConsoleWriter {
        int line = 0;
        int indent = 0;
        Vec2i console_screen_pos;
    public:
        explicit ConsoleWriter(Vec2i console_screen_pos) : console_screen_pos(console_screen_pos) {};
        void ModifyIndent(int size);
        void AddLine(std::string text, int fg_color = COLOR_WHITE, int bg_color = COLOR_BLACK);
    };
}


#endif //PIRATESSIMULATION_NCURSESUTIL_H
