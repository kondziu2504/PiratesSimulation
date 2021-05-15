//
// Created by konrad on 14.05.2021.
//

#include <unistd.h>
#include <thread>
#include <iostream>
#include "Ship.h"
using namespace std;

Ship::Ship(Vec2 pos, shared_ptr<World> world){
    this->pos = pos;
    this->world = world;
    direction = Vec2(5, 2).Normalized();
    velocity = 0.25f;
}

Vec2 Ship::GetPos() {
    lock_guard<mutex> guard(pos_mutex);
    return pos;
}

void Ship::Start() {
    thread shipThread(&Ship::UpdateThread, this);
    shipThread.detach();
}

[[noreturn]] void Ship::UpdateThread() {
    while(true){
        {
            lock_guard<mutex> guard(pos_mutex);
            pos = pos + direction * velocity;
        }
        usleep(100000);
    }
}

Vec2 Ship::GetDir() {
    lock_guard<mutex> guard(pos_mutex);
    return direction;
}
