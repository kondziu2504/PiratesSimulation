//
// Created by konrad on 16.05.2021.
//

#include <cmath>
#include "Mast.h"

using namespace std;

float Mast::GetAngle() {
    lock_guard<mutex> guard(mast_mutex);
    return angle;
}

void Mast::AdjustAngle(float angle_delta) {
    lock_guard<mutex> guard(mast_mutex);
    this->angle += angle_delta;
    while(this->angle > 2 * M_PI)
        this->angle -= 2 * M_PI;
    while(this->angle < 0)
        this->angle += 2 * M_PI;
}

float Mast::GetMaxSlots() {
    return max_slots;
}
