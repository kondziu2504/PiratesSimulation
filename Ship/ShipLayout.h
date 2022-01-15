//
// Created by konrad on 01.01.2022.
//

#ifndef PIRATESSIMULATION_SHIPLAYOUT_H
#define PIRATESSIMULATION_SHIPLAYOUT_H



#include "Mast.h"
#include "Cannon.h"

class ShipLayout {
    std::vector<std::unique_ptr<Mast>> masts;
    std::unique_ptr<ShipObject> left_junction;
    std::unique_ptr<ShipObject> right_junction;
    std::vector<std::unique_ptr<Cannon>> left_cannons;
    std::vector<std::unique_ptr<Cannon>> right_cannons;
    std::unique_ptr<ShipObject> resting_point;

public:
    ShipLayout(int masts_count, int cannons_per_side, float ship_length, WorldObject * parent);

    ShipObject * GetLeftJunction();
    ShipObject * GetRightJunction();
    std::vector<Cannon *> GetLeftCannons();
    std::vector<Cannon *> GetRightCannons();
    ShipObject * GetRestingPoint();
    std::vector<Mast *> GetMasts();
};


#endif //PIRATESSIMULATION_SHIPLAYOUT_H
