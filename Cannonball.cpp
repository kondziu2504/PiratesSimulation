//
// Created by konrad on 18.05.2021.
//

#include <unistd.h>
#include "Cannonball.h"
#include <thread>

using namespace std;

Cannonball::Cannonball(Vec2 origin, Vec2 target) {
    this->origin = origin;
    this->target = target;

    thread cannon_thread(&Cannonball::CannonThread, this);
    cannon_thread.detach();
}

void Cannonball::CannonThread() {
    while(true){
        {
            lock_guard<mutex> guard(progress_mutex);
            progress += 0.1f;
            if(progress >= 1.f){
                dead = true;
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
