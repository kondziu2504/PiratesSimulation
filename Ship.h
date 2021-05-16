//
// Created by konrad on 14.05.2021.
//

#ifndef PIRATESSIMULATION_SHIP_H
#define PIRATESSIMULATION_SHIP_H


#include "Vec2.h"
#include <mutex>
#include <memory>
#include <vector>

class MastDistributor;
class Mast;
class World;
class Sailor;

class Ship {

    Vec2 pos;
    std::shared_ptr<World> world;
    std::mutex pos_mutex;
    Vec2 direction;

    void AdjustDirection();
    [[noreturn]] void UpdateThread();
public:
    std::shared_ptr<std::vector<std::shared_ptr<Sailor>>> sailors;
    std::mutex sailors_mutex;
    std::shared_ptr<std::vector<std::shared_ptr<Mast>>> masts;
    std::shared_ptr<MastDistributor> distributor;

    Ship(Vec2 pos, Vec2 direction, std::shared_ptr<World> world);
    void ApplyWind(Vec2 wind);
    Vec2 GetPos();
    Vec2 GetDir();
    void Start();
};


#endif //PIRATESSIMULATION_SHIP_H
