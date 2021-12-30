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

using namespace std;

Ship::Ship(Vec2 pos, Vec2 direction, int sailors_count, int masts_count, int cannons_per_side, World * world) : sailors(
        GenerateSailors(sailors_count)){
    this->pos = pos;
    this->world = world;
    this->direction = direction.Normalized();
    stairs = make_shared<Stairs>();
    masts = make_shared<vector<shared_ptr<Mast>>>();
    left_junction = make_shared<ShipObject>();
    right_junction = make_shared<ShipObject>();
    for(int i = 0; i < masts_count; i++)
        masts->push_back(make_shared<Mast>(this));
    distributor = make_shared<MastDistributor>(masts);
    for(int i = 0; i < cannons_per_side; i++){
        right_cannons.push_back(make_shared<Cannon>(this, (float)(i + 1) / cannons_per_side));
        left_cannons.push_back(make_shared<Cannon>(this, (float)(i + 1) / cannons_per_side));
    }
}

Vec2 Ship::GetPos() {
    lock_guard<mutex> guard(pos_mutex);
    return pos;
}

void Ship::Start() {
    thread shipThread(&Ship::UpdateThread, this);
    shipThread.detach();
}

void Ship::UpdateThread() {
    while(true){
        ShipState current_state = GetState();
        if(current_state == ShipState::kDestroyed)
            return;
        if(current_state == ShipState::kWandering){
            LookForEnemy();
            AdjustDirection();
            SleepSeconds(0.1f);
        }else if(current_state == ShipState::kFighting){
            GetInPosition();
            if(enemy->GetState() == ShipState::kSinking || enemy->GetState() == ShipState::kDestroyed) {
                enemy = nullptr;
                SetState(ShipState::kWandering);
            }
        }
    }
}

Vec2 Ship::GetDir() {
    lock_guard<mutex> guard(pos_mutex);
    return direction;
}

void Ship::AdjustDirection() {
    const int scan_dist = 9;
    int closest_tile_dist = scan_dist;
    Vec2 total_correction(0, 0);
    total_correction += CalculateCorrectionAgainstLand(scan_dist, closest_tile_dist);
    total_correction += CalculateCorrectionAgainstShips(scan_dist, closest_tile_dist);
    ApplyCorrection(scan_dist, closest_tile_dist, total_correction);
}

void Ship::ApplyWind(Vec2 wind) {
    if(GetState() == ShipState::kFighting)
            return;
    float wind_power = wind.Length();
    float effective_power = 0;
    for(shared_ptr<Mast> mast : *masts){
        Vec2 absolute_mast_dir = Vec2::FromAngle(GetDir().Angle() + mast->GetAngle());
        float mast_effectiveness = absolute_mast_dir.Dot(wind.Normalized()) / 6;
        effective_power += wind_power * mast_effectiveness;
    }

    effective_power = max(effective_power, 0.05f);
    lock_guard<mutex> guard(pos_mutex);
    pos = pos + direction * effective_power;
}

ShipState Ship::GetState() {
    lock_guard<mutex> guard(state_mutex);
    return state;
}

void Ship::SetState(ShipState new_state) {
    lock_guard<mutex> guard(state_mutex);
    state = new_state;
}

void Ship::EngageFight(Ship * enemy_ship) {
    if(enemy_ship->GetState() == ShipState::kFighting)
        return;
    PrepareForFight(enemy_ship);
    enemy_ship->PrepareForFight(this);
}

void Ship::PrepareForFight(Ship *ship) {
    enemy = ship;
    SetState(ShipState::kFighting);
}

bool Ship::LookForEnemy() {
    lock_guard<mutex> guard(world->shipsMutex);
    for(auto ship : world->ships){
        if(ship.get() != this &&
        ship->GetState() != ShipState::kSinking &&
        ship->GetState() != ShipState::kDestroyed){
            int lookout_radius = 9;

            float dist = (ship->GetPos() - GetPos()).Length();
            Vec2 enemy_pos = ship->GetPos();
            Vec2 this_pos = GetPos();
            if(dist < lookout_radius){
                EngageFight(ship.get());
                return true;
            }
        }
    }
    return false;
}

void Ship::GetInPosition() {
    float target_angle = (enemy->GetPos() - GetPos()).Angle() - M_PI_2;
    //TODO: use_right_cannons = angle_diff > 0;
    use_right_cannons = false;
    float current_angle = GetDir().Angle();
    float angle_diff = AngleDifference(target_angle, current_angle);;
    while(abs(current_angle - target_angle) > M_PI / 180.f * 5.f){
        float angle_diff = AngleDifference(target_angle, current_angle);
        float angle_change = min(abs(angle_diff), 0.1f);
        {
            lock_guard<mutex> guard(pos_mutex);
            direction = direction.Rotated(angle_change * (angle_diff > 0 ? 1 : -1));
        }
        current_angle = GetDir().Angle();
        usleep(100000);
    }

}

void Ship::Destroy(bool respawn) {
    auto shipState = GetState();
    if(shipState == ShipState::kSinking || shipState == ShipState::kDestroyed)
        return;
    SetState(ShipState::kSinking);
    thread destroyThread([this, respawn]{
        vector<thread> killThreads;
        for(auto sailor : *sailors)
            killThreads.emplace_back(thread([=]() {sailor->Kill();}));

        for(auto & killThread : killThreads)
            killThread.join();

        SetState(ShipState::kDestroyed);
        {
            lock_guard<mutex> guard(world->shipsMutex);
            auto it = world->ships.begin();
            while (it != world->ships.end()) {
                if (it->get() == this) {
                    world->ships.erase(it);
                    break;
                }
                it++;
            }
        }
        if(respawn)
            world->GenerateShip();
    });
    destroyThread.detach();
}

int Ship::GetHP() const {
    return hp;
}

void Ship::Hit(int damage) {
    hp = max(0, hp - damage);
}

bool Ship::GetUseRightCannons() const {
    return use_right_cannons;
}

float Ship::GetLength() const {
    return length;
}

std::shared_ptr<ShipObject> Ship::GetLeftJunction() {
    return left_junction;
}

std::shared_ptr<ShipObject> Ship::GetRightJunction() {
    return right_junction;
}

Ship *Ship::GetEnemy() {
    return enemy;
}

std::vector<std::shared_ptr<Cannon>> Ship::GetLeftCannons() {
    return left_cannons;
}

std::vector<std::shared_ptr<Cannon>> Ship::GetRightCannons() {
    return right_cannons;
}

std::shared_ptr<Stairs> Ship::GetStairs() {
    return stairs;
}

std::shared_ptr<ShipObject> Ship::GetRestingPoint() {
    return resting_point;
}

World *Ship::GetWorld() {
    return world;
}

vector<shared_ptr<Sailor>> Ship::GetSailors() {
    return *sailors;
}

vector<shared_ptr<Mast>> Ship::GetMasts() {
    return *masts;
}

shared_ptr<MastDistributor> Ship::GetMastDistributor() {
    return distributor;
}

shared_ptr<vector<shared_ptr<Sailor>>> Ship::GenerateSailors(int sailors_count) {
    auto generated_sailors = make_shared<vector<shared_ptr<Sailor>>>();
    for(int i = 0; i < sailors_count; i++){
        shared_ptr<Sailor> sailor = make_shared<Sailor>(this);
        sailor->Start();
        generated_sailors->push_back(sailor);
    }
    return generated_sailors;
}

Vec2 Ship::CalculateCorrectionAgainstLand(int scan_dist, int& closest_tile_dist) const {
    Vec2 correction(0, 0);
    for(int j = 0; j < 16; j++){
        Vec2 checkDir = Vec2::FromAngle((float)j/16 * 2 * M_PI);
        for(int i = 0; i < scan_dist; i++){
            Vec2 scannedTile = pos + checkDir * i;
            scannedTile.x = (int)scannedTile.x;
            scannedTile.y = (int)scannedTile.y;
            bool tileOutsideWorld = scannedTile.x < 0 || scannedTile.x >= world->width ||
                                    scannedTile.y < 0 || scannedTile.y > world->height;
            if(world->map[scannedTile.y * world->width + scannedTile.x] || tileOutsideWorld){
                if(!(pos - scannedTile).Length() == 0){
                    Vec2 correction_dir = (pos - scannedTile).Normalized();
                    float significance = (float)(scan_dist - i) / scan_dist;
                    closest_tile_dist = min(closest_tile_dist, i);
                    correction = correction + correction_dir * significance;
                }
            }
        }
    }
    return correction;
}

Vec2 Ship::CalculateCorrectionAgainstShips(int scan_dist, int &closest_tile_dist) const {
    Vec2 correction;
    lock_guard<mutex> guard(world->shipsMutex);
    for(shared_ptr<Ship> ship : world->ships){
        if(&(*ship) != &(*this)){
            Vec2 ship_pos = ship->GetPos();
            float dist = (pos - ship_pos).Length();
            if(dist < scan_dist && dist > 0){
                Vec2 correction_dir = (pos - ship_pos).Normalized();
                float significance = (float)(scan_dist - dist) / scan_dist;
                closest_tile_dist = min(closest_tile_dist, (int)dist);
                correction = correction + correction_dir * significance;
            }
        }
    }
    return correction;
}

void Ship::ApplyCorrection(int scan_dist, int closest_tile_dist, Vec2 correction) {
    if(correction.Length() != 0){
        Vec2 normalized_correction = correction.Normalized();
        float correction_significance = (float)(scan_dist - closest_tile_dist) / scan_dist;
        if(direction.Dot(normalized_correction) > 0)
            correction_significance = 0;
        direction = direction + normalized_correction * correction_significance;
        direction = direction.Normalized();
    }
}

