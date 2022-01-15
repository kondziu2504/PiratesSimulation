//
// Created by konrad on 15.05.2021.
//

#ifndef PIRATESSIMULATION_WIND_H
#define PIRATESSIMULATION_WIND_H


#include "Util/Vec2f.h"
#include "Stoppable.h"
#include <memory>
#include <mutex>
#include <atomic>
class World;

class Wind : public Stoppable {
    Vec2f velocity = Vec2f(1,0);
    World * world;
    std::mutex wind_mutex;

    void ThreadFunc(const std::atomic<bool> &stop_requested) override;

    void UpdateVelocity();
public:
    explicit Wind(World * world);
    Vec2f GetVelocity();
};


#endif //PIRATESSIMULATION_WIND_H
