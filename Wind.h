//
// Created by konrad on 15.05.2021.
//

#ifndef PIRATESSIMULATION_WIND_H
#define PIRATESSIMULATION_WIND_H


#include "Util/Vec2.h"
#include <memory>
#include <mutex>
class World;

class Wind {
    Vec2 velocity = Vec2(1,0);
    World * world;

    std::mutex wind_mutex;

    [[noreturn]] void ThreadFun();
    void UpdateVelocity();
public:
    Vec2 GetVelocity();
    Wind(World * world);
    void Start();
};


#endif //PIRATESSIMULATION_WIND_H
