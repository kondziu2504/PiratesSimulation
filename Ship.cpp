//
// Created by konrad on 14.05.2021.
//

#include "Ship.h"
using namespace std;

Ship::Ship(Pos pos) {
    this->pos = pos;
}

Pos Ship::GetPos() {
    lock_guard<mutex> guard(pos_mutex);
    return pos;
}
