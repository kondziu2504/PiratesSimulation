//
// Created by konrad on 18.05.2021.
//

#include <unistd.h>
#include "Cannonball.h"
#include <thread>
#include "World.h"
#include "Ship.h"
using namespace std;

Cannonball::Cannonball(World * world, Vec2 origin, Vec2 target) {
    this->origin = origin;
    this->target = target;
    this->world = world;

    thread cannon_thread(&Cannonball::CannonThread, this);
    cannon_thread.detach();
}

void Cannonball::CannonThread() {
    while(true){
        {
            float progress_copy;
            {
                lock_guard<mutex> guard(progress_mutex);
                progress += 0.1f;
                progress_copy = progress;
            }

            if(progress_copy >= 1.f){
                dead = true;
                for(auto ship : world->ships)
                    if((ship->GetPosition() - GetPos()).Length() < 3)
                        ship->Hit(1);
                return;
            }
        }
        usleep(100000);
    }
}

Vec2 Cannonball::GetPos() {
    lock_guard<mutex> guard(progress_mutex);
    return origin + (target - origin) * progress;
}
