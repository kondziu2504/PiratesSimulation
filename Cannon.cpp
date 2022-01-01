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
        lock_guard<mutex> guard_cannonballs(parent->GetWorld()->cannonballs_mutex);
        Vec2 origin_offset = local_pos.Rotated(parent->GetDirection().Angle());
        parent->GetWorld()->cannonballs.push_back(make_shared<Cannonball>(parent->GetWorld(), parent->GetPosition() + origin_offset, target + origin_offset));
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

Cannon::Cannon(Vec2 local_pos, WorldObject * parent) {
    this->parent = parent;
    this->local_pos = local_pos;
    owners.first = owners.second = nullptr;
}
