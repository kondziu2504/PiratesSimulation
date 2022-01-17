//
// Created by konrad on 16.05.2021.
//

#include <cmath>
#include "Mast.h"
#include "Ship.h"

using namespace std;

float Mast::GetAngle() const {
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

int Mast::GetMaxSlots() const {
    return max_slots;
}

void Mast::Adjust() {
    float wind_angle = parent->GetWorld()->GetWind()->GetVelocity().Angle();
    float absolute_mast_angle = (parent->GetDirection().Angle() - (float)M_PI_2) + GetAngle();
    float angle_diff = AngleDifference(wind_angle, absolute_mast_angle);
    float angle_change = min(abs(angle_diff), (float)M_PI / 180.f);
    angle_diff = angle_change * (angle_diff >= 0.f ? 1.f : -1.f);
    AdjustAngle(angle_diff);
}

