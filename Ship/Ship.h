//
// Created by konrad on 14.05.2021.
//

#ifndef PIRATESSIMULATION_SHIP_H
#define PIRATESSIMULATION_SHIP_H


#include "../Util/Vec2f.h"
#include <mutex>
#include <memory>
#include <vector>
#include "ShipObject.h"
#include "../WorldObject.h"
#include "ShipLayout.h"
#include "ShipBody.h"
#include "Crew.h"
#include "ShipController.h"
#include <memory>
#include <atomic>

class MastDistributor;
class Mast;
class World;
class Sailor;
class Cannon;

class Ship : public WorldObject, public Stoppable {
    std::shared_ptr<ShipBody> ship_body;
    std::shared_ptr<Crew> crew;
    std::shared_ptr<ShipController> ship_controller;

    void ThreadFunc(const std::atomic<bool> &stop_requested) override;

public:
    Ship(Vec2f pos, Vec2f direction, int sailors, int masts, int cannons_per_side, World * world);

    void ApplyWind(Vec2f wind);
    void Hit(int damage);

    int GetHP() const;
    float GetLength() const;
    ShipState GetState();
    std::vector<std::shared_ptr<Sailor>> GetSailors();

    std::shared_ptr<ShipObject> GetLeftJunction();
    std::shared_ptr<ShipObject> GetRightJunction();
    std::vector<std::shared_ptr<Cannon>> GetLeftCannons();
    std::vector<std::shared_ptr<Cannon>> GetRightCannons();
    std::shared_ptr<ShipObject> GetRestingPoint();
    std::shared_ptr<MastDistributor> GetMastDistributor();
    std::vector<std::shared_ptr<Mast>> GetMasts();
    void PrepareForFight(Ship * enemy);
};


#endif //PIRATESSIMULATION_SHIP_H
