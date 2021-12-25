//
// Created by konrad on 25.12.2021.
//

#ifndef PIRATESSIMULATION_STAIRS_H
#define PIRATESSIMULATION_STAIRS_H


#include <mutex>
#include "ShipObject.h"

class Stairs : public ShipObject {
public:
    std::mutex mutex;
    Stairs(ShipObjectIdGenerator * shipObjectIdGenerator) : ShipObject(shipObjectIdGenerator) {}
};


#endif //PIRATESSIMULATION_STAIRS_H
