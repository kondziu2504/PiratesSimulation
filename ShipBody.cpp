//
// Created by konrad on 01.01.2022.
//

#include "ShipBody.h"
#include "MastDistributor.h"

using namespace std;

int ShipBody::GetHP() const {
    return hp;
}

void ShipBody::Hit(int damage) {
    hp = max(0, hp - damage);
}

float ShipBody::GetLength() const {
    return length;
}

ShipBody::ShipBody(WorldObject * parent, int hp, int length, int mast_count, int cannons_per_side) {
    this->hp = hp;
    this->length = length;
    ship_layout = make_shared<ShipLayout>(mast_count, cannons_per_side, length, parent);
    distributor = make_shared<MastDistributor>(ship_layout->GetMasts());
}

std::shared_ptr<ShipObject> ShipBody::GetLeftJunction() {
    return ship_layout->GetLeftJunction();
}

std::shared_ptr<ShipObject> ShipBody::GetRightJunction() {
    return ship_layout->GetRightJunction();
}

std::vector<std::shared_ptr<Cannon>> ShipBody::GetLeftCannons() {
    return ship_layout->GetLeftCannons();
}

std::vector<std::shared_ptr<Cannon>> ShipBody::GetRightCannons() {
    return ship_layout->GetRightCannons();
}

std::shared_ptr<Stairs> ShipBody::GetStairs() {
    return ship_layout->GetStairs();
}

std::shared_ptr<ShipObject> ShipBody::GetRestingPoint() {
    return ship_layout->GetRestingPoint();
}

std::vector<std::shared_ptr<Mast>> ShipBody::GetMasts() {
    return ship_layout->GetMasts();
}

std::shared_ptr<MastDistributor> ShipBody::GetMastDistributor() {
    return distributor;
}
