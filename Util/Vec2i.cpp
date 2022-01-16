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

Vec2i::Vec2i(Vec2f vec2) {
    x = (int)vec2.x;
    y = (int)vec2.y;
}

Vec2i Vec2i::operator-(const Vec2i &second) const {
    return {this->x - second.x, this->y - second.y};
}

Vec2f Vec2i::operator*(const float & second) const {
    return {(float)this->x * second, (float)this->y * second};
}

Vec2i::operator Vec2f() const {
    return {(float)this->x, (float)this->y};
}

Vec2i Vec2i::operator-() const {
    return {-this->x, -this->y};
}
