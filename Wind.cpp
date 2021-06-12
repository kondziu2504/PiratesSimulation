//
// Created by konrad on 15.05.2021.
//

#include "Wind.h"
#include <thread>
#include <unistd.h>
#include "World.h"
#include "Ship.h"

using namespace std;

void Wind::Start() {
    thread wind_thread(&Wind::ThreadFun, this);
    wind_thread.detach();
}

[[noreturn]] void Wind::ThreadFun() {
    while(true){
        UpdateVelocity();
        {
            lock_guard<mutex> guard(world->shipsMutex);
            for(shared_ptr<Ship> ship : world->ships){
                auto shipState = ship->GetState();
                if(shipState == ShipState::kRoaming)
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

}

Vec2 Wind::GetVelocity() {
    lock_guard<mutex> guard(wind_mutex);
    return velocity;
}
