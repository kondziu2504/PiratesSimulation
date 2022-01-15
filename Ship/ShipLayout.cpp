//
// Created by konrad on 01.01.2022.
//

#include "ShipLayout.h"
#include <memory>
#include "Mast.h"
#include "Cannon.h"
#include "../Util/Util.h"

using namespace std;

ShipLayout::ShipLayout(int masts_count, int cannons_per_side, float ship_length, WorldObject * parent) {
    masts = vector<unique_ptr<Mast>>();
    ShipLayout::left_junction = make_unique<ShipObject>();
    ShipLayout::right_junction = make_unique<ShipObject>();
    for(int i = 0; i < masts_count; i++)
        masts.push_back(make_unique<Mast>(parent));
    for(int i = 0; i < cannons_per_side; i++){
        Vec2f local_canon_pos = parent->GetDirection() * -1 * ship_length * (((float)i + 0.5f) / (float)cannons_per_side - 0.5f);
        ShipLayout::right_cannons.push_back(make_unique<Cannon>(local_canon_pos, parent));
        ShipLayout::left_cannons.push_back(make_unique<Cannon>(local_canon_pos, parent));
    }
}

ShipObject * ShipLayout::GetLeftJunction() {
    return left_junction.get();
}

ShipObject * ShipLayout::GetRightJunction() {
    return right_junction.get();
}

vector<Cannon *> ShipLayout::GetLeftCannons() {
    return MapToRawPointers(left_cannons);
}

vector<Cannon *> ShipLayout::GetRightCannons() {
    return MapToRawPointers(right_cannons);
}

ShipObject * ShipLayout::GetRestingPoint() {
    return resting_point.get();
}

vector<Mast *> ShipLayout::GetMasts() {
    return MapToRawPointers(masts);
}

