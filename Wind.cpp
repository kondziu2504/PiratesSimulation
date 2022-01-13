//
// Created by konrad on 15.05.2021.
//

#include "Wind.h"
#include <thread>
#include <unistd.h>
#include "World.h"
#include "Ship/Ship.h"

using namespace std;

void Wind::Start() {
    thread wind_thread(&Wind::ThreadFun, this);
    wind_thread.detach();
}

[[noreturn]] void Wind::ThreadFun() {
    while(true){
        UpdateVelocity();
        {
            for(const shared_ptr<Ship>& ship : world->GetShips()){
                auto shipState = ship->GetState();
                if(shipState == ShipState::kWandering)
                    ship->ApplyWind(velocity);
            }
        }
        usleep(100000);
    }
}

Wind::Wind(World * world) {
    this->world = world;
}

void Wind::UpdateVelocity() {
    float currentAngle = velocity.Angle();
    float newAngle = currentAngle + (((float)rand() / RAND_MAX) - 0.5f) / 3.f;
    velocity = Vec2f::FromAngle(newAngle);
}

Vec2f Wind::GetVelocity() {
    lock_guard<mutex> guard(wind_mutex);
    return velocity;
}
