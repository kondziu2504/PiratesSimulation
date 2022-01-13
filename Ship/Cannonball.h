//
// Created by konrad on 18.05.2021.
//

#ifndef PIRATESSIMULATION_CANNONBALL_H
#define PIRATESSIMULATION_CANNONBALL_H

#include <mutex>
#include "../Util/Vec2f.h"
class World;

class Cannonball {
    float progress = 0.f;
    Vec2f origin;
    Vec2f target;

    World * world;

    std::mutex progress_mutex;

    void CannonThread();
public:
    bool dead = false;
    Cannonball(World * world, Vec2f origin, Vec2f target);
    Vec2f GetPos();
};


#endif //PIRATESSIMULATION_CANNONBALL_H
