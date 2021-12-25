//
// Created by konrad on 25.12.2021.
//

#ifndef PIRATESSIMULATION_SHIPOBJECTIDGENERATOR_H
#define PIRATESSIMULATION_SHIPOBJECTIDGENERATOR_H

#include <atomic>

class ShipObjectIdGenerator {
    std::atomic<int> unoccupiedId = 1;

public:
    ShipObjectIdGenerator() = default;
    ShipObjectIdGenerator(const ShipObjectIdGenerator&) = delete;

    static const int kNoObject;
    int GetUniqueId();
};


#endif //PIRATESSIMULATION_SHIPOBJECTIDGENERATOR_H
