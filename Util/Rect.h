//
// Created by konrad on 09.01.2022.
//

#ifndef PIRATESSIMULATION_RECT_H
#define PIRATESSIMULATION_RECT_H


#include "Vec2i.h"
#include "Vec2f.h"

struct Rect {
    Vec2i pos;
    Vec2i size;

    Rect(int x, int y, int width, int height);
    Rect(Vec2i pos, Vec2i size);

    bool IsPointInside(Vec2f point);
};


#endif //PIRATESSIMULATION_RECT_H
