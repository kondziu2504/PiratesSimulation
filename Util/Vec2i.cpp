//
// Created by konrad on 09.01.2022.
//

#include "Vec2i.h"

Vec2i::Vec2i(int x, int y) :
        x(x),
        y(y) { }

Vec2i Vec2i::operator+(const Vec2i &second) const {
    return {this->x + second.x, this->y + second.y};
}
