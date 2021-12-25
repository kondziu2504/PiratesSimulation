//
// Created by konrad on 16.05.2021.
//

#ifndef PIRATESSIMULATION_MAST_H
#define PIRATESSIMULATION_MAST_H


#include <mutex>
#include <condition_variable>
#include "ShipObject.h"
#include "Operable.h"

class MastDistributor;


class Mast : public ShipObject, public Operable {
    int max_slots = 4;
    float angle = 0.f;

    Ship * ship;

    std::mutex mast_mutex;
public:
    [[nodiscard]] int GetMaxSlots() const;
    float GetAngle();
    void AdjustAngle(float angle_delta);
    explicit Mast(Ship * ship);
    void Operate(Sailor * sailor) override;
};


#endif //PIRATESSIMULATION_MAST_H
