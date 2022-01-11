//
// Created by konrad on 09.01.2022.
//

#ifndef PIRATESSIMULATION_RECT_H
#define PIRATESSIMULATION_RECT_H


#include "Vec2i.h"
#include "Vec2.h"

struct Rect {
    int x;
    int y;
    int width;
    int height;

    Rect(int x, int y, int width, int height);

    bool IsPointInside(Vec2 point);
};


#endif //PIRATESSIMULATION_RECT_H
