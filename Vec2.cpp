//
// Created by konrad on 14.05.2021.
//

#include <valarray>
#include "Vec2.h"

Vec2::Vec2(float x, float y) {
    this->x = x;
    this->y = y;
}

float Vec2::Distance() {
    return sqrt(x * x + y * y);
}

Vec2 Vec2::operator+(const Vec2 &b) {
    return Vec2(this->x + b.x, this->y + b.y);
}

Vec2 Vec2::operator*(float b) {
    return Vec2(this->x * b, this->y * b);
}

Vec2 Vec2::Normalized() {
    return (*this) / Distance();
}

Vec2 Vec2::operator/(float b) {
    if(b == 0)
        throw std::runtime_error("Division by 0");
    return Vec2(this->x / b, this->y / b);
}

Vec2 Vec2::Rotate(float angle) {
    float s = sin(angle), c = cos(angle);
    return Vec2(
            this->x * c - this->y * s,
            this->x * s + this->y * c);
}

float Vec2::Angle() {
    return atan2(this->y, this->x);
}
