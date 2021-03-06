//
// Created by konrad on 14.05.2021.
//

#include <thread>
#include <cmath>
#include "Ship.h"
#include "Mast.h"
#include "MastDistributor.h"
#include "Sailor.h"
#include "Cannon.h"
#include "Crew.h"
#include "ShipController.h"

using namespace std;

Ship::Ship(Vec2f pos, Vec2f direction, int sailors_count, int masts_count, int cannons_per_side, World * world) :
        WorldObject(direction.Normalized(), pos, world),
        ship_body(move(make_unique<ShipBody>(this, 10, 6, masts_count, cannons_per_side))),
        crew(move(make_unique<Crew>(sailors_count, ship_body.get(), this))),
        ship_controller(move(make_unique<ShipController>(crew.get(), this)))
{
}

void Ship::ApplyWind(Vec2f wind) {
    if(GetState() == ShipState::kFighting)
            return;
    float wind_power = wind.Length();
    float effective_power = 0;
    for(const auto mast : ship_body->GetMasts()){
        Vec2f absolute_mast_dir = Vec2f::FromAngle(GetDirection().Angle() - (float)M_PI_2 + mast->GetAngle());
        float mast_effectiveness = absolute_mast_dir.Dot(wind.Normalized()) / 6;
        effective_power += wind_power * mast_effectiveness;
    }

    const float MIN_EFFECTIVE_POWER = 0.05f;
    effective_power = max(effective_power, MIN_EFFECTIVE_POWER);

    SetPosition(GetPosition() + GetDirection() * effective_power);
}

int Ship::GetHP() const {
    return ship_body->GetHP();
}

void Ship::Hit(int damage) {
    ship_body->Hit(damage);
    if(ship_body->GetHP() <= 0){
        RequestStop();
    }
}

float Ship::GetLength() const {
    return ship_body->GetLength();
}

vector<Sailor *> Ship::GetSailors() const {
    return crew->GetSailors();
}

ShipObject * Ship::GetLeftJunction() const {
    return ship_body->GetLeftJunction();
}

ShipObject * Ship::GetRightJunction() const {
    return ship_body->GetRightJunction();
}

std::vector<Cannon *> Ship::GetLeftCannons() const {
    return ship_body->GetLeftCannons();
}

std::vector<Cannon *> Ship::GetRightCannons() const {
    return ship_body->GetRightCannons();
}

ShipObject * Ship::GetRestingPoint() const {
    return ship_body->GetRestingPoint();
}

MastDistributor * Ship::GetMastDistributor() const {
    return ship_body->GetMastDistributor();
}

std::vector<Mast *> Ship::GetMasts() const {
    return ship_body->GetMasts();
}

ShipState Ship::GetState() const {
    return ship_controller->GetState();
}

void Ship::PrepareForFight(const weak_ptr<Ship>& enemy) {
    ship_controller->PrepareForFight(enemy.lock());
}

void Ship::ThreadFunc(const atomic<bool> &stop_requested) {
    ship_controller->Start();
    crew->Start();

    WaitUntilStopRequested();

    crew->RequestStop();
    ship_controller->RequestStop();

    crew->WaitUntilStopped();
    ship_controller->WaitUntilStopped();
}


