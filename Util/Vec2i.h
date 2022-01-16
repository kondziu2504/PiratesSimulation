//
// Created by konrad on 09.01.2022.
//

#ifndef PIRATESSIMULATION_VEC2I_H
#define PIRATESSIMULATION_VEC2I_H


#include "Vec2f.h"

struct Vec2i {
    explicit Vec2i(Vec2f vec2);
    explicit operator Vec2f() const;

    int x, y;

    Vec2i(int x, int y);
    Vec2i() : Vec2i(0, 0) {};

    Vec2i operator+(const Vec2i & second) const;
    Vec2i operator-(const Vec2i & second) const;
    Vec2f operator*(const float & second) const;
    Vec2i operator-() const;
};


#endif //PIRATESSIMULATION_VEC2I_H
