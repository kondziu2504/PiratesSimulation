//
// Created by konrad on 14.05.2021.
//

#include <valarray>
#include <stdexcept>
#include "Vec2f.h"

Vec2f::Vec2f(float x, float y) {
    this->x = x;
    this->y = y;
}

float Vec2f::Length() const {
    return sqrtf(x * x + y * y);
}

Vec2f Vec2f::Normalized() const {
    return (*this) / Length();
}


Vec2f Vec2f::Rotated(float angle) const {
    float s = sinf(angle), c = cosf(angle);
    return {
            this->x * c - this->y * s,
            this->x * s + this->y * c};
}

float Vec2f::Angle() const {
    return atan2f(this->y, this->x);
}


Vec2f Vec2f::FromAngle(float angle) {
    return {cosf(angle), sinf(angle)};
}

float Vec2f::Dot(const Vec2f & b) const {
    return this->x * b.x + this->y * b.y;
}

Vec2f Vec2f::operator/(float b) const {
    if(b == 0)
        throw std::runtime_error("Division by 0");
    return {this->x / b, this->y / b};
}

Vec2f Vec2f::operator+(const Vec2f &b) const {
    return {this->x + b.x, this->y + b.y};
}

Vec2f Vec2f::operator*(float b) const {
    return {this->x * b, this->y * b};
}

Vec2f Vec2f::operator-(const Vec2f &b) const {
    return {this->x - b.x, this->y - b.y};
}

Vec2f &Vec2f::operator+=(const Vec2f &b) {
    *this = *this + b;
    return *this;
}
