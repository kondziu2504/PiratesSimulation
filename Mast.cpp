//
// Created by konrad on 16.05.2021.
//

#include "Mast.h"

using namespace std;

bool Mast::AddOccupant() {
    lock_guard<mutex> guard(mast_mutex);
    if(occupied_slots < max_slots){
        occupied_slots++;
        return true;
    }else{
        return false;
    }
}

void Mast::RemoveOccupant() {
    lock_guard<mutex> guard(mast_mutex);
    occupied_slots--;
}

float Mast::GetEfficiency() {
    lock_guard<mutex> guard(mast_mutex);
    return (float)occupied_slots / max_slots;
}

float Mast::GetAngle() {
    lock_guard<mutex> guard(mast_mutex);
    return angle;
}

void Mast::SetAngle(float angle) {
    lock_guard<mutex> guard(mast_mutex);
    this->angle = angle;
}
