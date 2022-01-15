//
// Created by konrad on 01.01.2022.
//

#ifndef PIRATESSIMULATION_SHIPLAYOUT_H
#define PIRATESSIMULATION_SHIPLAYOUT_H



#include "Mast.h"
#include "Cannon.h"

class ShipLayout {
    const std::vector<std::unique_ptr<Mast>> masts;
    const std::unique_ptr<ShipObject> left_junction;
    const std::unique_ptr<ShipObject> right_junction;
    const std::vector<std::unique_ptr<Cannon>> left_cannons;
    const std::vector<std::unique_ptr<Cannon>> right_cannons;
    const std::unique_ptr<ShipObject> resting_point;

    static std::vector<std::unique_ptr<Mast>> GenerateMasts(int masts_count, WorldObject * parent);
    static std::vector<std::unique_ptr<Cannon>> GenerateCannons(int num_of_cannons, float ship_length, WorldObject * parent);

public:
    ShipLayout(int masts_count, int cannons_per_side, float ship_length, WorldObject * parent);

    ShipObject * GetLeftJunction() const;
    ShipObject * GetRightJunction() const;
    std::vector<Cannon *> GetLeftCannons() const;
    std::vector<Cannon *> GetRightCannons() const;
    ShipObject * GetRestingPoint() const;
    std::vector<Mast *> GetMasts() const;
};


#endif //PIRATESSIMULATION_SHIPLAYOUT_H
