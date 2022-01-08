//
// Created by konrad on 14.05.2021.
//

#include <valarray>
#include <stdexcept>
#include "Vec2.h"

Vec2::Vec2(float x, float y) {
    this->x = x;
    this->y = y;
}

float Vec2::Length() const {
    return sqrt(x * x + y * y);
}

Vec2 Vec2::Normalized() const {
    return (*this) / Length();
}


Vec2 Vec2::Rotated(float angle) const {
    float s = sin(angle), c = cos(angle);
    return Vec2(
            this->x * c - this->y * s,
            this->x * s + this->y * c);
}

float Vec2::Angle() const {
    return atan2(this->y, this->x);
}


Vec2 Vec2::FromAngle(float angle) {
    return Vec2(cos(angle), sin(angle));
}

float Vec2::Dot(const Vec2 & b) const {
    return this->x * b.x + this->y * b.y;
}

Vec2 Vec2::operator/(float b) const {
    if(b == 0)
        throw std::runtime_error("Division by 0");
    return Vec2(this->x / b, this->y / b);
}

Vec2 Vec2::operator+(const Vec2 &b) const {
    return Vec2(this->x + b.x, this->y + b.y);
}

Vec2 Vec2::operator*(float b) const {
    return Vec2(this->x * b, this->y * b);
}

Vec2 Vec2::operator-(const Vec2 &b) const {
    return Vec2(this->x - b.x, this->y - b.y);
}

Vec2 &Vec2::operator+=(const Vec2 &b) {
    *this = *this + b;
    return *this;
}
