//
// Created by konrad on 01.01.2022.
//

#include "ShipLayout.h"
#include <memory>
#include "Mast.h"
#include "Cannon.h"
#include "../Util/Util.h"

using namespace std;

ShipLayout::ShipLayout(int masts_count, int cannons_per_side, float ship_length, WorldObject * parent) :
        masts(move(GenerateMasts(masts_count, parent))),
        left_junction(move(make_unique<ShipObject>())),
        right_junction(move(make_unique<ShipObject>())),
        resting_point(move(make_unique<ShipObject>())),
        left_cannons(move(GenerateCannons(cannons_per_side, ship_length, parent))),
        right_cannons(move(GenerateCannons(cannons_per_side, ship_length, parent)))
{
}

ShipObject * ShipLayout::GetLeftJunction() const {
    return left_junction.get();
}

ShipObject * ShipLayout::GetRightJunction() const {
    return right_junction.get();
}

vector<Cannon *> ShipLayout::GetLeftCannons() const {
    return MapToRawPointers(left_cannons);
}

vector<Cannon *> ShipLayout::GetRightCannons() const {
    return MapToRawPointers(right_cannons);
}

ShipObject * ShipLayout::GetRestingPoint() const {
    return resting_point.get();
}

vector<Mast *> ShipLayout::GetMasts() const{
    return MapToRawPointers(masts);
}

std::vector<std::unique_ptr<Mast>> ShipLayout::GenerateMasts(int masts_count, WorldObject * parent) {
    auto masts = vector<unique_ptr<Mast>>();
    for(int i = 0; i < masts_count; i++)
        masts.push_back(move(make_unique<Mast>(parent)));
    return move(masts);
}

std::vector<std::unique_ptr<Cannon>> ShipLayout::GenerateCannons(int num_of_cannons, float ship_length, WorldObject *parent) {
    auto cannons = vector<unique_ptr<Cannon>>();
    for(int i = 0; i < num_of_cannons; i++){
        Vec2f local_canon_pos = parent->GetDirection() * -1 * ship_length * (((float)i + 0.5f) / (float)num_of_cannons - 0.5f);
        cannons.push_back(move(make_unique<Cannon>(local_canon_pos, parent)));
    }
    return move(cannons);
}

