//
// Created by konrad on 18.05.2021.
//

#include <unistd.h>
#include "Cannonball.h"
#include <thread>
#include "../World.h"
#include "Ship.h"
using namespace std;

Cannonball::Cannonball(World * world, Vec2f origin, Vec2f target) {
    this->origin = origin;
    this->target = target;
    this->world = world;
}

Vec2f Cannonball::GetPos() {
    return origin + (target - origin) * progress;
}

void Cannonball::ThreadFunc(const atomic<bool> &stop_requested) {
    while(true){
        progress = progress + 0.1f;
        if(progress >= 1.f){
            for(const auto& ship : world->GetShips()) {
                if((ship->GetPosition() - GetPos()).Length() < 3){
                    ship->Hit(1);
                    break;
                }
            }
            dead = true;
            return;
        }
        SleepSeconds(0.1f);
    }
}
