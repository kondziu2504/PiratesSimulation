//
// Created by konrad on 09.01.2022.
//

#include "Rect.h"

Rect::Rect(int x, int y, int width, int height) :
        pos(x, y),
        size(width, height) {}

bool Rect::IsPointInside(Vec2f point) {
    return point.x >= pos.x && point.x < pos.x + size.x
           && point.y >= pos.y && point.y < pos.y + size.y;
}

Rect::Rect(Vec2i pos, Vec2i size) : pos(pos), size(size) {}
