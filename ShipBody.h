//
// Created by konrad on 01.01.2022.
//

#ifndef PIRATESSIMULATION_SHIPBODY_H
#define PIRATESSIMULATION_SHIPBODY_H


#include "ShipLayout.h"

class ShipBody {
    int hp;
    float length;
    std::shared_ptr<ShipLayout> ship_layout;
    std::shared_ptr<MastDistributor> distributor;

public:
    explicit ShipBody(WorldObject * parent, int hp = 10, int length = 6, int mast_count = 3, int cannons_per_side = 3);

    [[nodiscard]] int GetHP() const;
    void Hit(int damage);
    [[nodiscard]] float GetLength() const;
    std::shared_ptr<MastDistributor> GetMastDistributor();

    std::shared_ptr<ShipObject> GetLeftJunction();
    std::shared_ptr<ShipObject> GetRightJunction();
    std::vector<std::shared_ptr<Cannon>> GetLeftCannons();
    std::vector<std::shared_ptr<Cannon>> GetRightCannons();
    std::shared_ptr<Stairs> GetStairs();
    std::shared_ptr<ShipObject> GetRestingPoint();
    std::vector<std::shared_ptr<Mast>> GetMasts();
};


#endif //PIRATESSIMULATION_SHIPBODY_H
