//
// Created by konrad on 09.01.2022.
//

#ifndef PIRATESSIMULATION_NCURSESUTIL_H
#define PIRATESSIMULATION_NCURSESUTIL_H

#define COLOR_BROWN 8
#define COLOR_GRAY 9

namespace ncurses_util {
    void Initialize();
    short CursColor(int fg);
    int ColorNum(int fg, int bg);
    void InitAllPossibleColorPairs();
}


#endif //PIRATESSIMULATION_NCURSESUTIL_H
