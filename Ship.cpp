//
// Created by konrad on 14.05.2021.
//

#include <unistd.h>
#include <thread>
#include <iostream>
#include <cmath>
#include "Ship.h"
#include "World.h"
#include "Mast.h"
#include "MastDistributor.h"
#include "Sailor.h"
#include "Util.h"
#include "Cannon.h"
#include "Stairs.h"
#include "ShipLayout.h"
#include "Crew.h"
#include "ShipController.h"

using namespace std;

Ship::Ship(Vec2 pos, Vec2 direction, int sailors_count, int masts_count, int cannons_per_side, World * world) :
        WorldObject(direction.Normalized(), pos, world)
{
    const int kShipHealth = 10;
    const int kShipLength = 6;
    ship_body = make_shared<ShipBody>(this, kShipHealth, kShipLength, masts_count, cannons_per_side);
    crew = make_shared<Crew>(sailors_count, ship_body.get(), this);
    ship_controller = make_shared<ShipController>(ship_body.get(), crew.get(), this);
}

void Ship::Start() {
    thread ship_thread(&ShipController::Start, ship_controller);
    thread crew_thread(&Crew::Start, crew);
    ship_thread.join();
    crew_thread.join();
}

void Ship::ApplyWind(Vec2 wind) {
    if(GetState() == ShipState::kFighting)
            return;
    float wind_power = wind.Length();
    float effective_power = 0;
    for(shared_ptr<Mast> mast : ship_body->GetMasts()){
        Vec2 absolute_mast_dir = Vec2::FromAngle(GetDirection().Angle() + mast->GetAngle());
        float mast_effectiveness = absolute_mast_dir.Dot(wind.Normalized()) / 6;
        effective_power += wind_power * mast_effectiveness;
    }

    const float MIN_EFFECTIVE_POWER = 0.05f;
    effective_power = max(effective_power, MIN_EFFECTIVE_POWER);

    SetPosition(GetPosition() + GetDirection() * effective_power);
}

void Ship::Destroy(bool respawn) {
    if(ship_controller->GetState() == ShipState::kSinking || ship_controller->GetState() == ShipState::kDestroyed)
        return;

    thread kill_thread([=](){
        crew->Kill();
        ship_controller->Kill();
    });
    kill_thread.detach();
}

int Ship::GetHP() const {
    return ship_body->GetHP();
}

void Ship::Hit(int damage) {
    ship_body->Hit(damage);
    if(ship_body->GetHP() <= 0){
        Destroy(true);
    }
}

float Ship::GetLength() const {
    return ship_body->GetLength();
}

vector<shared_ptr<Sailor>> Ship::GetSailors() {
    return crew->GetSailors();
}

std::shared_ptr<ShipObject> Ship::GetLeftJunction() {
    return ship_body->GetLeftJunction();
}

std::shared_ptr<ShipObject> Ship::GetRightJunction() {
    return ship_body->GetRightJunction();
}

std::vector<std::shared_ptr<Cannon>> Ship::GetLeftCannons() {
    return ship_body->GetLeftCannons();
}

std::vector<std::shared_ptr<Cannon>> Ship::GetRightCannons() {
    return ship_body->GetRightCannons();
}

std::shared_ptr<Stairs> Ship::GetStairs() {
    return ship_body->GetStairs();
}

std::shared_ptr<ShipObject> Ship::GetRestingPoint() {
    return ship_body->GetRestingPoint();
}

std::shared_ptr<MastDistributor> Ship::GetMastDistributor() {
    return ship_body->GetMastDistributor();
}

std::vector<std::shared_ptr<Mast>> Ship::GetMasts() {
    return ship_body->GetMasts();
}

ShipState Ship::GetState() {
    return ship_controller->GetState();
}

void Ship::PrepareForFight(Ship *enemy) {
    ship_controller->PrepareForFight(enemy);
}


