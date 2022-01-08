//
// Created by konrad on 01.01.2022.
//

#ifndef PIRATESSIMULATION_SHIPLAYOUT_H
#define PIRATESSIMULATION_SHIPLAYOUT_H



#include "Mast.h"
#include "Cannon.h"

class ShipLayout {
    std::shared_ptr<std::vector<std::shared_ptr<Mast>>> masts;
    std::shared_ptr<ShipObject> left_junction;
    std::shared_ptr<ShipObject> right_junction;
    std::vector<std::shared_ptr<Cannon>> left_cannons;
    std::vector<std::shared_ptr<Cannon>> right_cannons;
    std::shared_ptr<ShipObject> resting_point;

public:
    ShipLayout(int masts_count, int cannons_per_side, float ship_length, WorldObject * parent);

    std::shared_ptr<ShipObject> GetLeftJunction();
    std::shared_ptr<ShipObject> GetRightJunction();
    std::vector<std::shared_ptr<Cannon>> GetLeftCannons();
    std::vector<std::shared_ptr<Cannon>> GetRightCannons();
    std::shared_ptr<ShipObject> GetRestingPoint();
    std::vector<std::shared_ptr<Mast>> GetMasts();
};


#endif //PIRATESSIMULATION_SHIPLAYOUT_H
