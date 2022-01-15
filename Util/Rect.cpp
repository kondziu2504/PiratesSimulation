//
// Created by konrad on 09.01.2022.
//

#include "Rect.h"

Rect::Rect(int x, int y, int width, int height) :
        pos(x, y),
        size(width, height) {}

bool Rect::IsPointInside(Vec2f point) const {
    return point.x >= (float)pos.x && point.x < (float)pos.x + (float)size.x
           && point.y >= (float)pos.y && point.y < (float)pos.y + (float)size.y;
}

Rect::Rect(Vec2i pos, Vec2i size) : pos(pos), size(size) {}
