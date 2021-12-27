//
// Created by konrad on 18.05.2021.
//

#include <iostream>
#include "Cannon.h"
#include "World.h"
#include "Cannonball.h"

using namespace std;

void Cannon::Load() {
    {
        lock_guard<mutex> guard(loaded_mutex);
        loaded = true;
    }
    c_var_loaded.notify_one();
}

void Cannon::Shoot(Vec2 target) {
    lock_guard<mutex> guard(loaded_mutex);
    loaded = true;
    {
        lock_guard<mutex> guard_cannonballs(ship->world->cannonballs_mutex);
        Vec2 origin_offset = ship->GetDir() * ship->GetLength() * (0.5 - relative_pos_along);
        ship->world->cannonballs.push_back(make_shared<Cannonball>(ship->world, ship->GetPos() + origin_offset, target + origin_offset));
    }
    loaded = false;
}

bool Cannon::TryClaim(Sailor * sailor) {
    lock_guard<mutex> guard(ownership_mutex);
    if(owners.first == nullptr){
        owners.first = sailor;
        return true;
    }else if(owners.second == nullptr){
        owners.second = sailor;
        return true;
    }
    return false;
}

bool Cannon::Loaded() {
    lock_guard<mutex> guard(loaded_mutex);
    return loaded;
}

void Cannon::WaitUntilLoadedOrTimeout() {
    unique_lock<mutex> lock(loaded_mutex);
    //c_var_loaded.wait(lock, [&]{return loaded;});
    c_var_loaded.wait_for(lock, chrono::seconds(5), [&]{return loaded;});
    lock.unlock();
}

std::pair<Sailor *, Sailor *> Cannon::GetOwners() {
    lock_guard<mutex> guard(ownership_mutex);
    return owners;
}

void Cannon::Release(Sailor *sailor) {
    lock_guard<mutex> guard(ownership_mutex);
    if(owners.first == sailor)
        owners.first = nullptr;
    else if(owners.second == sailor)
        owners.second = nullptr;
}

Cannon::Cannon(Ship * ship, float relative_pos_along) {
    this->ship = ship;
    this->relative_pos_along = relative_pos_along;
    owners.first = owners.second = nullptr;
}
