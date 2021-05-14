//
// Created by konrad on 14.05.2021.
//

#ifndef PIRATESSIMULATION_SHIP_H
#define PIRATESSIMULATION_SHIP_H


#include "Pos.h"
#include <mutex>

class Ship {
    Pos pos;

    std::mutex pos_mutex;

public:
    Ship(Pos pos);
    Pos GetPos();

};


#endif //PIRATESSIMULATION_SHIP_H
