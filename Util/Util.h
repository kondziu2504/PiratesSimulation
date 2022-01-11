//
// Created by konrad on 17.05.2021.
//

#ifndef PIRATESSIMULATION_UTIL_H
#define PIRATESSIMULATION_UTIL_H

#include <functional>
#include <string>
#include "../Ship/Sailor.h"

float AngleDifference(float angle1, float angle2);
float RandomTime(float min, float max);
void SleepSeconds(float seconds);
void DoRepeatedlyForATime(const std::function<void(float progress, bool & stop)>& action, float totalTimeInSeconds, float updatePeriodInSeconds = 0.1f);

#endif //PIRATESSIMULATION_UTIL_H
