//
// Created by konrad on 16.05.2021.
//

#include "Mast.h"

using namespace std;

float Mast::GetAngle() {
    lock_guard<mutex> guard(mast_mutex);
    return angle;
}

void Mast::SetAngle(float angle) {
    lock_guard<mutex> guard(mast_mutex);
    this->angle = angle;
}

float Mast::GetMaxSlots() {
    return max_slots;
}
