//
// Created by konrad on 09.01.2022.
//

#ifndef PIRATESSIMULATION_VEC2I_H
#define PIRATESSIMULATION_VEC2I_H


struct Vec2i {
    int x, y;

    Vec2i(int x, int y);

    Vec2i operator+(const Vec2i & second) const;
};


#endif //PIRATESSIMULATION_VEC2I_H
