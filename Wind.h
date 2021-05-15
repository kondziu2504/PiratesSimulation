//
// Created by konrad on 15.05.2021.
//

#ifndef PIRATESSIMULATION_WIND_H
#define PIRATESSIMULATION_WIND_H


#include "Vec2.h"
#include <memory>
class World;

class Wind {
    Vec2 velocity = Vec2(1,0);
    World * world;

    [[noreturn]] void ThreadFun();
    void UpdateVelocity();
public:
    Wind(World * world);
    void Start();
};


#endif //PIRATESSIMULATION_WIND_H
