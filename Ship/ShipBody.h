//
// Created by konrad on 01.01.2022.
//

#ifndef PIRATESSIMULATION_SHIPBODY_H
#define PIRATESSIMULATION_SHIPBODY_H


#include "ShipLayout.h"
#include "MastDistributor.h"

class ShipBody {
    int hp;
    const float length;
    const std::unique_ptr<ShipLayout> ship_layout;
    const std::unique_ptr<MastDistributor> distributor;

public:
    explicit ShipBody(WorldObject * parent, int hp = 10, float length = 6, int mast_count = 3, int cannons_per_side = 3);

    int GetHP() const;
    void Hit(int damage);
    float GetLength() const;
    MastDistributor * GetMastDistributor() const;

    ShipObject * GetLeftJunction() const;
    ShipObject * GetRightJunction() const;
    std::vector<Cannon *> GetLeftCannons() const;
    std::vector<Cannon *> GetRightCannons() const;
    ShipObject * GetRestingPoint() const;
    std::vector<Mast *> GetMasts() const;
};


#endif //PIRATESSIMULATION_SHIPBODY_H