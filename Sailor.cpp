//
// Created by konrad on 16.05.2021.
//

#include <thread>
#include <unistd.h>
#include <iostream>
#include "Sailor.h"
#include "Mast.h"
#include "Ship.h"
#include "Wind.h"
#include "World.h"
#include "MastDistributor.h"
#include "Util.h"
#include "Cannon.h"

using namespace std;

const std::uniform_real_distribution<double> Sailor::distribution = std::uniform_real_distribution(4.0, 6.0);

void Sailor::Start() {
    thread sailor_thread(&Sailor::ThreadFun, this);
    sailor_thread.detach();
}

void Sailor::ThreadFun() {
    usleep(RandomTime(0,10) * 1000000);
    while(true){
        ShipState ship_state = ship->GetState();
        if(ship_state == ShipState::kWandering) {
            GoUseMastProcedure();
        }else if(ship_state == ShipState::kFighting){
            GoUseCannonProcedure();
        }
        if (dying) {
            SetState(SailorState::kDead);
            return;
        }
        GoRest();
        //GoUseStairs();
    }
}


void Sailor::GoRest() {
    GoTo(ship->restingPoint);
    SetState(SailorState::kResting);
    usleep(RandomTime(3,5) * 1000000);
    SetState(SailorState::kStanding);
}

Sailor::Sailor(Ship * ship) {
    this->ship = ship;
}

SailorState Sailor::GetState() {
    lock_guard<mutex> guard(sailor_mutex);
    return currentState;
}

void Sailor::SetState(SailorState new_state) {
    lock_guard<mutex> guard(sailor_mutex);
    currentState = new_state;
}

void Sailor::OperateMast() {
    SetState(SailorState::kMast);
    for(int i = 0; i < 60; i++){
        usleep(100000);
        SetProgress((float)i / 9);
        operated_mast->Adjust();
    }

    ship->distributor->ReleaseMast(operated_mast, this);
    {
        lock_guard<mutex> guard(sailor_mutex);
        operated_mast = nullptr;
    }
    SetState(SailorState::kStanding);
}

void Sailor::WaitForMast() {
    SetState(SailorState::kWaitingMast);
    operated_mast = ship->distributor->RequestMast(this);
    SetState(SailorState::kStanding);
}

float Sailor::GetProgress() {
    lock_guard<mutex> guard(sailor_mutex);
    return activity_progress;
}

void Sailor::SetProgress(float progress) {
    lock_guard<mutex> guard(sailor_mutex);
    activity_progress = progress;
}

std::shared_ptr<ShipObject> Sailor::GetPreviousTarget() {
    lock_guard<mutex> guard(target_mutex);
    return previous_target;
}

std::shared_ptr<ShipObject> Sailor::GetNextTarget() {
    lock_guard<mutex> guard(target_mutex);
    return next_target;
}

void Sailor::UseStairs() {
    SetState(SailorState::kWaitingStairs);
    lock_guard<mutex> guard(ship->stairs->mutex);
    SetState(SailorState::kStairs);
    for(int i = 0; i < 10; i++){
        SetProgress((float)i / 9);
        usleep(100000);
    }
    {
        lock_guard<mutex> guard(sailor_mutex);
        upper_deck = !upper_deck;
    }
    SetState(SailorState::kStanding);
}

void Sailor::GoUseStairs() {
    SetState(SailorState::kWalking);
    next_target = ship->stairs;
    for(int i = 0; i < 10; i++){
        SetProgress((float)i / 9);
        usleep(100000);
    }
    previous_target = next_target;
    SetState(SailorState::kStanding);

    UseStairs();
}

bool Sailor::IsUpperDeck() {
    lock_guard<mutex> guard(sailor_mutex);
    return upper_deck;
}

void Sailor::WaitForCannon() {
    SetState(SailorState::kWaitingCannon);
    while(true){
        auto side_cannons = ship->use_right_cannons ? ship->right_cannons : ship->left_cannons;
        for(auto cannon : side_cannons){
            if(cannon->TryClaim(this)){
                assigned_cannon = cannon;
                SetState(SailorState::kWaitingCannon);
                return;
            }
        }
        usleep(100000);
    }
}

void Sailor::UseCannon() {
    SetState(SailorState::kCannon);
    usleep(2000000);
    auto cannon_owners = assigned_cannon->GetOwners();
    if(cannon_owners.first == this){
        assigned_cannon->WaitUntilLoaded();
        if(assigned_cannon->Loaded()) {
            float distance = 5;
            if(ship->enemy != nullptr)
                distance = (ship->enemy->GetPos() - ship->GetPos()).Distance();
            Vec2 perpendicular = Vec2::FromAngle(ship->GetDir().Angle() - M_PI_2).Normalized() * distance;
            assigned_cannon->Shoot(ship->GetPos() + perpendicular * (ship->use_right_cannons ? 1 : -1));
        }
    }else if(cannon_owners.second == this){
        assigned_cannon->Load();
    }

    assigned_cannon->Release(this);
    SetState(SailorState::kStanding);
}

void Sailor::GoTo(shared_ptr<ShipObject> shipObject) {
    SetState(SailorState::kWalking);
    const float kWalkDuration = 3.f;
    next_target = shipObject;
    float seconds_per_step = 0.05;
    int steps = kWalkDuration / seconds_per_step;
    for(int i = 0; i < steps; i++){
        SetProgress((float)i / (steps - 1));
        usleep(seconds_per_step * 1000000);
    }
    previous_target = next_target;
    SetState(SailorState::kStanding);
}

void Sailor::Kill(){
    dying = true;
    while(GetState() != SailorState::kDead);
}

std::shared_ptr<Mast> Sailor::GetOperatedMast() const {
    return operated_mast;
}

std::shared_ptr<Cannon> Sailor::GetOperatedCannon() const {
    return assigned_cannon;
}

bool Sailor::GetIsDying() const {
    return dying;
}

std::shared_ptr<ShipObject> Sailor::GetFightingSideJunction() const {
    return ship->use_right_cannons ? ship->right_junction : ship->left_junction;
}

void Sailor::GoUseMastProcedure() {
    GoTo(ship->right_junction);
    WaitForMast();
    GoTo(operated_mast);
    OperateMast();
}

void Sailor::GoUseCannonProcedure() {
    GoTo(GetFightingSideJunction());
    WaitForCannon();
    GoTo(assigned_cannon);
    UseCannon();
}



