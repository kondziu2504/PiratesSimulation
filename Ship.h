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
class Cannon;

enum class ShipState{kRoaming, kFighting};

class Ship {

    Vec2 pos;
    std::mutex pos_mutex;
    Vec2 direction;
    ShipState state = ShipState::kRoaming;
    std::mutex state_mutex;

    void SetState(ShipState new_state);

    void EngageFight(Ship * ship);
    void PrepareForFight(Ship * ship);
    bool LookForEnemy();
    void GetInPosition();
    void AdjustDirection();
    [[noreturn]] void UpdateThread();
public:
    int hp = 10;
    bool use_right_cannons = true;
    std::shared_ptr<void> left_junction;
    std::shared_ptr<void> right_junction;
    float length = 6;
    ShipState GetState();
    Ship * enemy = nullptr;
    std::vector<std::shared_ptr<Cannon>> right_cannons;
    std::vector<std::shared_ptr<Cannon>> left_cannons;
    std::shared_ptr<std::mutex> stairs_mutex;

    std::shared_ptr<World> world;
    std::shared_ptr<std::vector<std::shared_ptr<Sailor>>> sailors;
    std::mutex sailors_mutex;
    std::shared_ptr<std::vector<std::shared_ptr<Mast>>> masts;
    std::shared_ptr<MastDistributor> distributor;

    Ship(Vec2 pos, Vec2 direction, int sailors, int masts, int cannons_per_side, std::shared_ptr<World> world);
    void ApplyWind(Vec2 wind);
    Vec2 GetPos();
    Vec2 GetDir();
    void Start();
};


#endif //PIRATESSIMULATION_SHIP_H
