//
// Created by konrad on 14.05.2021.
//

#ifndef PIRATESSIMULATION_SHIP_H
#define PIRATESSIMULATION_SHIP_H


#include "Pos.h"
#include <mutex>
#include <memory>

class World;

class Ship {

    Pos pos;
    std::shared_ptr<World> world;

    std::mutex pos_mutex;


    [[noreturn]] void UpdateThread();
public:
    Ship(Pos pos, std::shared_ptr<World> world);

    Pos GetPos();
    void Start();
};


#endif //PIRATESSIMULATION_SHIP_H
