//
// Created by konrad on 16.05.2021.
//

#ifndef PIRATESSIMULATION_MAST_H
#define PIRATESSIMULATION_MAST_H


#include <mutex>
#include <condition_variable>
#include "ShipObject.h"
#include "../Wind.h"
#include "../WorldObject.h"

class MastDistributor;
class Ship;

class Mast : public ShipObject{
    const int max_slots = 4;
    float angle = 0.f;

    WorldObject * const parent;
    mutable std::mutex mast_mutex;

    void AdjustAngle(float angle_delta);

public:
    explicit Mast(WorldObject * parent) : parent(parent) {};

    int GetMaxSlots() const;
    float GetAngle() const;
    void Adjust();
};


#endif //PIRATESSIMULATION_MAST_H
