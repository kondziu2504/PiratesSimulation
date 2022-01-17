//
// Created by konrad on 18.05.2021.
//

#include "Cannon.h"
#include "Cannonball.h"
#include "../Util/Util.h"

using namespace std;

Cannon::Cannon(Vec2f local_pos, WorldObject * parent) :
        local_pos(local_pos) ,
        parent(parent) {
    owners.first = owners.second = nullptr;
}


void Cannon::Load() {
    {
        lock_guard<mutex> guard(load_mutex);
        loaded = true;
    }
    c_var_loaded_or_released.notify_one();
}

void Cannon::Shoot(Vec2f target) {
    lock_guard<mutex> guard(load_mutex);
    Vec2f origin_offset = local_pos.Rotated(parent->GetDirection().Angle());
    parent->GetWorld()->AddCannonball(make_shared<Cannonball>(parent->GetWorld(), parent->GetPosition() + origin_offset, target + origin_offset));
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

bool Cannon::Loaded() const {
    return loaded;
}

void Cannon::WaitUntilLoadedOrTimeout(const std::function<bool()>& StopCondition) const {
    const float kTimeoutTime = 5;
    const float kStepSeconds = 0.1f;
    float time_left = kTimeoutTime;
    while(time_left > 0) {
        unique_lock<mutex> lock(load_mutex);
        bool predicate_true = c_var_loaded_or_released.wait_for(lock, chrono::milliseconds((int)(kStepSeconds * 1000)) , [&]{return loaded.load();});
        time_left -= kStepSeconds;
        if(predicate_true || StopCondition())
            return;
    }
}

std::pair<Sailor *, Sailor *> Cannon::GetOwners() const {
    lock_guard<mutex> guard(ownership_mutex);
    return owners;
}

void Cannon::Release(Sailor *sailor) {
    lock_guard<mutex> guard(ownership_mutex);
    if(owners.first == sailor){
        owners.first = nullptr;
        if(owners.second != nullptr) {
            owners.first = owners.second;
            owners.second = nullptr;
        }
    }
    else if(owners.second == sailor)
        owners.second = nullptr;
}

