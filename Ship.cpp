//
// Created by konrad on 14.05.2021.
//

#include <unistd.h>
#include <thread>
#include <iostream>
#include "Ship.h"
using namespace std;

Ship::Ship(Pos pos, shared_ptr<World> world){
    this->pos = pos;
    this->world = world;
}

Pos Ship::GetPos() {
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
            pos = Pos(pos.x + 0.13f, pos.y);
        }
        usleep(100000);
    }
}
