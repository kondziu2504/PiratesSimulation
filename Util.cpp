//
// Created by konrad on 17.05.2021.
//

#include <cmath>
#include "Util.h"
#include <random>
#include <mutex>
#include <unistd.h>

std::mt19937 mt;

using namespace std;

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

mutex rand_mutex;

float RandomTime(float min, float max){
    lock_guard<mutex> guard(rand_mutex);
    std::uniform_real_distribution dis(min, max);
    return dis(mt);
}

float SleepSeconds(float seconds){
    usleep(1'000'000);
}
