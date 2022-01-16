//
// Created by konrad on 17.05.2021.
//

#include <cmath>
#include "Util.h"
#include <random>
#include <mutex>
#include <unistd.h>

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


std::mt19937 mt;
mutex rand_mutex;

float RandomFromRange(float min, float max){
    lock_guard<mutex> guard(rand_mutex);
    std::uniform_real_distribution dis(min, max);
    return dis(mt);
}


void SleepSeconds(float seconds){
    usleep((unsigned int)(1000000 * seconds));
}

void DoRepeatedlyForATime(const std::function<void(float progress, bool & stop)>& action, float totalTimeInSeconds, float updatePeriodInSeconds){
    if(totalTimeInSeconds <= 0 || updatePeriodInSeconds <= 0)
        return;


    float timeLeft = totalTimeInSeconds;
    bool stop = false;
    while(timeLeft > 0){
        if(action != nullptr)
            action(1.f - (timeLeft / totalTimeInSeconds), stop);
        if(stop)
            return;
        SleepSeconds(updatePeriodInSeconds);
        timeLeft -= updatePeriodInSeconds;
    }
}
