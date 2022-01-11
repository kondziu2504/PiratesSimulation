//
// Created by konrad on 09.01.2022.
//

#include "Rect.h"

Rect::Rect(int x, int y, int width, int height) :
        x(x),
        y(y),
        width(width),
        height(height) {}

bool Rect::IsPointInside(Vec2 point) {
    return point.x >= x && point.x < x + width
           && point.y >= y && point.y < y + height;
}
