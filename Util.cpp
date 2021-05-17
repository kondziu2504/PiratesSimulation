//
// Created by konrad on 17.05.2021.
//

#include <cmath>
#include "Util.h"

float AngleDifference(float angle1, float angle2){
    float angle_diff = angle1 - angle2;
    if(abs(angle_diff) > M_PI){
        if(angle_diff < 0)
            angle_diff += 2 * M_PI;
        else
            angle_diff -= 2 * M_PI;
    }
    return angle_diff;
}

