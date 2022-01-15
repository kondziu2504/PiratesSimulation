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

ShipBody::ShipBody(WorldObject * parent, int hp, float length, int mast_count, int cannons_per_side) {
    this->hp = hp;
    this->length = length;
    ship_layout = make_unique<ShipLayout>(mast_count, cannons_per_side, length, parent);
    distributor = make_unique<MastDistributor>(ship_layout->GetMasts());
}

ShipObject * ShipBody::GetLeftJunction() {
    return ship_layout->GetLeftJunction();
}

ShipObject * ShipBody::GetRightJunction() {
    return ship_layout->GetRightJunction();
}

std::vector<Cannon *> ShipBody::GetLeftCannons() {
    return ship_layout->GetLeftCannons();
}

std::vector<Cannon *> ShipBody::GetRightCannons() {
    return ship_layout->GetRightCannons();
}

ShipObject * ShipBody::GetRestingPoint() {
    return ship_layout->GetRestingPoint();
}

std::vector<Mast *> ShipBody::GetMasts() {
    return ship_layout->GetMasts();
}

MastDistributor * ShipBody::GetMastDistributor() {
    return distributor.get();
}
