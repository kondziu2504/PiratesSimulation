//
// Created by konrad on 16.05.2021.
//

#ifndef PIRATESSIMULATION_MAST_H
#define PIRATESSIMULATION_MAST_H


#include <mutex>
#include <condition_variable>
class MastDistributor;

class Mast {
    int max_slots = 4;
    float angle = 0.f;

    std::mutex mast_mutex;
public:
    float GetMaxSlots();
    float GetAngle();
    void AdjustAngle(float angle_delta);
};


#endif //PIRATESSIMULATION_MAST_H
