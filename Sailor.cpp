//
// Created by konrad on 16.05.2021.
//

#include <thread>
#include <unistd.h>
#include "Sailor.h"
#include "Mast.h"
#include "Ship.h"
#include "Wind.h"
#include "World.h"
#include "MastDistributor.h"
#include "Util.h"

using namespace std;

void Sailor::Start() {
    thread sailor_thread(&Sailor::ThreadFun, this);
    sailor_thread.detach();
}

[[noreturn]] void Sailor::ThreadFun() {
    while(true){
        GoWaitForMast();
        GoOperateMast();
        GoRest();
    }
}


void Sailor::GoRest() {
    next_target = nullptr;
    SetState(SailorState::kWalking);
    for(int i = 0; i < 10; i++){
        SetProgress((float)i / 9);
        usleep(100000);
    }
    SetState(SailorState::kStanding);
    previous_target = next_target;

    SetState(SailorState::kResting);
    usleep(1000000);
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
    for(int i = 0; i < 10; i++){
        usleep(100000);
        SetProgress((float)i / 9);
        float wind_angle = ship->world->wind->GetVelocity().Angle();
        float absolute_mast_angle = ship->GetDir().Angle() + operated_mast->GetAngle();
        float angle_diff = AngleDifference(wind_angle, absolute_mast_angle);
        float angle_change = min(abs(angle_diff), (float)M_PI / 180.f);
        angle_diff = angle_change * (angle_diff >= 0 ? 1 : -1);
        operated_mast->AdjustAngle(angle_diff);
    }

    ship->distributor->ReleaseMast(operated_mast, this);
    {
        lock_guard<mutex> guard(sailor_mutex);
        operated_mast = nullptr;
    }
    SetState(SailorState::kStanding);
}


void Sailor::GoOperateMast() {
    next_target = operated_mast;
    SetState(SailorState::kWalking);
    for(int i = 0; i < 10; i++){
        SetProgress((float)i / 9);
        usleep(100000);
    }
    SetState(SailorState::kStanding);
    previous_target = next_target;

    OperateMast();
}

void Sailor::WaitForMast() {
    SetState(SailorState::kWaitingMast);
    {
        lock_guard<mutex> guard(sailor_mutex);
        operated_mast = ship->distributor->RequestMast(this);
    }
    SetState(SailorState::kStanding);
}

void Sailor::GoWaitForMast() {
    next_target = ship->distributor;
    SetState(SailorState::kWalking);
    for(int i = 0; i < 10; i++){
        SetProgress((float)i / 9);
        usleep(100000);
    }
    previous_target = next_target;
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

void Sailor::SetPreviousTarget(std::shared_ptr<void> target) {
    lock_guard<mutex> guard(target_mutex);
    previous_target = target;
}

void Sailor::SetNextTarget(std::shared_ptr<void> target) {
    lock_guard<mutex> guard(target_mutex);
    next_target = target;
}

std::shared_ptr<void> Sailor::GetPreviousTarget() {
    lock_guard<mutex> guard(target_mutex);
    return previous_target;
}

std::shared_ptr<void> Sailor::GetNextTarget() {
    lock_guard<mutex> guard(target_mutex);
    return next_target;
}
