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
        usleep(1000000);
        {
            lock_guard<mutex> guard(sailor_mutex);
            operated_mast = ship->distributor->RequestMast(this);
        }
        SetState(SailorState::kMast);
        OperateMast();
        {
            lock_guard<mutex> guard(sailor_mutex);
            ship->distributor->ReleaseMast(operated_mast, this);
            operated_mast = nullptr;
        }
        SetState(SailorState::kResting);
    }
}


void Sailor::GoRest() {

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
    for(int i = 0; i < 10; i++){
        usleep(100000);
        float wind_angle = ship->world->wind->GetVelocity().Angle();
        float absolute_mast_angle = ship->GetDir().Angle() + operated_mast->GetAngle();
        float angle_diff = AngleDifference(wind_angle, absolute_mast_angle);
        float angle_change = min(abs(angle_diff), (float)M_PI / 180.f);
        angle_diff = angle_change * (angle_diff >= 0 ? 1 : -1);
        operated_mast->AdjustAngle(angle_diff);
    }
}
