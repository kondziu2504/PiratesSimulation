//
// Created by konrad on 18.05.2021.
//

#ifndef PIRATESSIMULATION_CANNONBALL_H
#define PIRATESSIMULATION_CANNONBALL_H

#include <mutex>
#include "Vec2.h"

class Cannonball {
    float progress = 0.f;
    Vec2 origin;
    Vec2 target;

    std::mutex progress_mutex;

    void CannonThread();
public:
    bool dead = false;
    Cannonball(Vec2 origin, Vec2 target);
    Vec2 GetPos();
};


#endif //PIRATESSIMULATION_CANNONBALL_H
