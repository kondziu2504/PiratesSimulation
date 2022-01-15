//
// Created by konrad on 01.01.2022.
//

#include "ShipLayout.h"
#include <memory>
#include "Mast.h"
#include "Cannon.h"

using namespace std;

ShipLayout::ShipLayout(int masts_count, int cannons_per_side, float ship_length, WorldObject * parent) {
    masts = make_shared<vector<shared_ptr<Mast>>>();
    ShipLayout::left_junction = make_shared<ShipObject>();
    ShipLayout::right_junction = make_shared<ShipObject>();
    for(int i = 0; i < masts_count; i++)
        masts->push_back(make_shared<Mast>(parent));
    for(int i = 0; i < cannons_per_side; i++){
        Vec2f local_canon_pos = parent->GetDirection() * -1 * ship_length * (((float)i + 0.5f) / (float)cannons_per_side - 0.5f);
        ShipLayout::right_cannons.push_back(make_shared<Cannon>(local_canon_pos, parent));
        ShipLayout::left_cannons.push_back(make_shared<Cannon>(local_canon_pos, parent));
    }
}

shared_ptr<ShipObject> ShipLayout::GetLeftJunction() {
    return left_junction;
}

shared_ptr<ShipObject> ShipLayout::GetRightJunction() {
    return right_junction;
}

vector<std::shared_ptr<Cannon>> ShipLayout::GetLeftCannons() {
    return left_cannons;
}

vector<std::shared_ptr<Cannon>> ShipLayout::GetRightCannons() {
    return right_cannons;
}

shared_ptr<ShipObject> ShipLayout::GetRestingPoint() {
    return resting_point;
}

vector<shared_ptr<Mast>> ShipLayout::GetMasts() {
    return *masts;
}

