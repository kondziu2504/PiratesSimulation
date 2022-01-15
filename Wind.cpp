//
// Created by konrad on 15.05.2021.
//

#include "Wind.h"
#include "World.h"
#include "Ship/Ship.h"

using namespace std;

Wind::Wind(World * world) : world(world){ }

void Wind::UpdateVelocity() {
    float currentAngle = velocity.Angle();
    float newAngle = currentAngle + (((float)rand() / RAND_MAX) - 0.5f) / 3.f;
    velocity = Vec2f::FromAngle(newAngle);
}

Vec2f Wind::GetVelocity() const {
    lock_guard<mutex> guard(wind_mutex);
    return velocity;
}

void Wind::ThreadFunc(const atomic<bool> &stop_requested) {
    while(true){
        UpdateVelocity();
        {
            for(const auto& ship : world->GetShips()){
                auto _ship = ship.lock();
                if(_ship){
                    auto shipState = _ship->GetState();
                    if(shipState == ShipState::kWandering)
                        _ship->ApplyWind(velocity);
                }
            }
        }
        SleepSeconds(0.1f);
        if(stop_requested)
            return;
    }
}
