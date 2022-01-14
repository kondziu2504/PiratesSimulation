//
// Created by konrad on 18.05.2021.
//

#ifndef PIRATESSIMULATION_CANNONBALL_H
#define PIRATESSIMULATION_CANNONBALL_H

#include <mutex>
#include <atomic>
#include "../Util/Vec2f.h"
#include "../Stopable.h"

class World;

class Cannonball : public Stopable{
    std::atomic<float> progress = 0.f;
    Vec2f origin;
    Vec2f target;
    World * world;

    void ThreadFunc(const std::atomic<bool> &stop_requested) override;

public:
    bool dead = false;
    Cannonball(World * world, Vec2f origin, Vec2f target);
    Vec2f GetPos();
};


#endif //PIRATESSIMULATION_CANNONBALL_H
