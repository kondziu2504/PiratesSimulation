//
// Created by konrad on 01.01.2022.
//

#ifndef PIRATESSIMULATION_SHIPBODY_H
#define PIRATESSIMULATION_SHIPBODY_H


#include "ShipLayout.h"

class ShipBody {
    int hp;
    float length;
    std::unique_ptr<ShipLayout> ship_layout;
    std::unique_ptr<MastDistributor> distributor;

public:
    explicit ShipBody(WorldObject * parent, int hp = 10, float length = 6, int mast_count = 3, int cannons_per_side = 3);

    int GetHP() const;
    void Hit(int damage);
    float GetLength() const;
    MastDistributor * GetMastDistributor();

    ShipObject * GetLeftJunction();
    ShipObject * GetRightJunction();
    std::vector<Cannon *> GetLeftCannons();
    std::vector<Cannon *> GetRightCannons();
    ShipObject * GetRestingPoint();
    std::vector<Mast *> GetMasts();
};


#endif //PIRATESSIMULATION_SHIPBODY_H