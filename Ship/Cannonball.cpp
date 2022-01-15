//
// Created by konrad on 18.05.2021.
//

#include "Cannonball.h"
#include "../World.h"
#include "Ship.h"
using namespace std;

Cannonball::Cannonball(World * world, Vec2f origin, Vec2f target) :
        origin(origin), target(target), world(world) {

}

Vec2f Cannonball::GetPos() const {
    return origin + (target - origin) * progress;
}

void Cannonball::ThreadFunc(const atomic<bool> &stop_requested) {
    while(true){
        progress = progress + 0.1f;
        if(progress >= 1.f){
            for(const auto& ship : world->GetShips()) {
                auto _ship = ship.lock();
                if(_ship){
                    if((_ship->GetPosition() - GetPos()).Length() < 3){
                        _ship->Hit(1);
                        break;
                    }
                }
            }
            dead = true;
            return;
        }
        SleepSeconds(0.1f);
    }
}
