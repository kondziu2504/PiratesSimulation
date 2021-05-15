//
// Created by konrad on 14.05.2021.
//

#ifndef PIRATESSIMULATION_SHIP_H
#define PIRATESSIMULATION_SHIP_H


#include "Vec2.h"
#include <mutex>
#include <memory>

class World;

class Ship {

    Vec2 pos;
    std::shared_ptr<World> world;

    std::mutex pos_mutex;

    Vec2 direction;
    float velocity;

    void AdjustDirection();
    [[noreturn]] void UpdateThread();
public:
    Ship(Vec2 pos, std::shared_ptr<World> world);

    Vec2 GetPos();
    Vec2 GetDir();
    void Start();
};


#endif //PIRATESSIMULATION_SHIP_H
