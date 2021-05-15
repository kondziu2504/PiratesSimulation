//
// Created by konrad on 16.05.2021.
//

#ifndef PIRATESSIMULATION_MAST_H
#define PIRATESSIMULATION_MAST_H


#include <mutex>

class Mast {
    int max_slots = 4;
    int occupied_slots = 0;
    float angle = 0.f;

    std::mutex mast_mutex;
public:
    bool AddOccupant();
    void RemoveOccupant();
    float GetEfficiency();
    float GetAngle();
    void SetAngle(float angle);
};


#endif //PIRATESSIMULATION_MAST_H
