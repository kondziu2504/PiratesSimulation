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
#include "ShipObjectIdGenerator.h"

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
        if(ship_state == ShipState::kRoaming) {
            GoWaitForMast();
            GoOperateMast();
        }else if(ship_state == ShipState::kFighting){
            GoWaitForCannon();
            GoUseCannon();
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
    SetState(SailorState::kWalking);
    Walk(ShipObjectIdGenerator::kNoObject, 3);
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
        operated_mast->Operate(this);
    }

    ship->distributor->ReleaseMast(operated_mast, this);
    {
        lock_guard<mutex> guard(sailor_mutex);
        operated_mast = nullptr;
    }
    SetState(SailorState::kStanding);
}


void Sailor::GoOperateMast() {
    SetState(SailorState::kWalking);
    Walk(operated_mast->GetId(), 2);
    SetState(SailorState::kStanding);
    previous_target = next_target;

    OperateMast();
}

void Sailor::WaitForMast() {
    SetState(SailorState::kWaitingMast);
    {
        operated_mast = ship->distributor->RequestMast(this);
    }
    SetState(SailorState::kStanding);
}

void Sailor::GoWaitForMast() {
    SetState(SailorState::kWalking);
    Walk(ship->right_junction->GetId(), 3);
    SetState(SailorState::kStanding);

    WaitForMast();
}

float Sailor::GetProgress() {
    lock_guard<mutex> guard(sailor_mutex);
    return activity_progress;
}

void Sailor::SetProgress(float progress) {
    lock_guard<mutex> guard(sailor_mutex);
    activity_progress = progress;
}

void Sailor::SetPreviousTarget(int target) {
    lock_guard<mutex> guard(target_mutex);
    previous_target = target;
}

void Sailor::SetNextTarget(int target) {
    lock_guard<mutex> guard(target_mutex);
    next_target = target;
}

int Sailor::GetPreviousTarget() {
    lock_guard<mutex> guard(target_mutex);
    return previous_target;
}

int Sailor::GetNextTarget() {
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
    next_target = ship->stairs->GetId();
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

void Sailor::GoWaitForCannon() {
    SetState(SailorState::kWalking);
    Walk(ship->use_right_cannons ? ship->right_junction->GetId() : ship->left_junction->GetId(), 3);
    SetState(SailorState::kStanding);

    WaitForCannon();
}

void Sailor::WaitForCannon() {
    SetState(SailorState::kWaitingCannon);
    while(true){
        auto side_cannons = ship->use_right_cannons ? ship->right_cannons : ship->left_cannons;
        for(auto cannon : side_cannons){
            if(cannon->TryClaim(this)){
                operated_cannon = cannon;
                SetState(SailorState::kWaitingCannon);
                return;
            }
        }
        usleep(100000);
    }
}

void Sailor::GoUseCannon() {
    SetState(SailorState::kWalking);
    Walk(operated_cannon->GetId(), 2);
    UseCannon();
}

void Sailor::UseCannon() {
    SetState(SailorState::kCannon);
    usleep(2000000);
    auto cannon_owners = operated_cannon->GetOwners();
    if(cannon_owners.first == this){
        operated_cannon->WaitUntilLoaded();
        if(operated_cannon->Loaded()) {
            float distance = 5;
            if(ship->enemy != nullptr)
                distance = (ship->enemy->GetPos() - ship->GetPos()).Distance();
            Vec2 perpendicular = Vec2::FromAngle(ship->GetDir().Angle() - M_PI_2).Normalized() * distance;
            operated_cannon->Shoot(ship->GetPos() + perpendicular * (ship->use_right_cannons ? 1 : -1));
        }
    }else if(cannon_owners.second == this){
        operated_cannon->Load();
    }

    operated_cannon->Release(this);
    SetState(SailorState::kStanding);
}

void Sailor::Walk(int next, float seconds) {
    next_target = next;
    float seconds_per_step = 0.05;
    int steps = seconds / seconds_per_step;
    for(int i = 0; i < steps; i++){
        SetProgress((float)i / (steps - 1));
        usleep(seconds_per_step * 1000000);
    }
    previous_target = next_target;
}

void Sailor::Kill(){
    dying = true;
    while(GetState() != SailorState::kDead);
}

std::shared_ptr<Mast> Sailor::GetOperatedMast() const {
    return operated_mast;
}

std::shared_ptr<Cannon> Sailor::GetOperatedCannon() const {
    return operated_cannon;
}

bool Sailor::GetIsDying() const{
    return dying;
}


