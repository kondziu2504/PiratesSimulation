//
// Created by konrad on 09.01.2022.
//

#ifndef PIRATESSIMULATION_VEC2I_H
#define PIRATESSIMULATION_VEC2I_H


#include "Vec2.h"

struct Vec2i {
    explicit Vec2i(Vec2 vec2);

    int x, y;

    Vec2i(int x, int y);
    Vec2i() : Vec2i(0, 0) {};

    Vec2i operator+(const Vec2i & second) const;
    Vec2i operator-(const Vec2i & second) const;
    Vec2 operator*(const float & second) const;
};


#endif //PIRATESSIMULATION_VEC2I_H
