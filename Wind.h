//
// Created by konrad on 15.05.2021.
//

#ifndef PIRATESSIMULATION_WIND_H
#define PIRATESSIMULATION_WIND_H


#include "Util/Vec2f.h"
#include <memory>
#include <mutex>
class World;

class Wind {
    Vec2f velocity = Vec2f(1,0);
    World * world;

    std::mutex wind_mutex;

    [[noreturn]] void ThreadFun();
    void UpdateVelocity();
public:
    Vec2f GetVelocity();
    Wind(World * world);
    void Start();
};


#endif //PIRATESSIMULATION_WIND_H
