//
// Created by konrad on 17.05.2021.
//

#ifndef PIRATESSIMULATION_UTIL_H
#define PIRATESSIMULATION_UTIL_H

#include <functional>
#include <string>
#include <memory>

float AngleDifference(float angle1, float angle2);
float RandomFromRange(float min, float max);
void SleepSeconds(float seconds);
void DoRepeatedlyForATime(const std::function<void(float progress, bool & stop)>& action, float totalTimeInSeconds, float updatePeriodInSeconds = 0.1f);

template<typename T>
std::vector<T *> MapToRawPointers(const std::vector<std::unique_ptr<T>> &unique_pointers) {
    std::vector<T *> raw_pointers(unique_pointers.size());
    std::transform(
            unique_pointers.begin(), unique_pointers.end(),
            raw_pointers.begin(),
            [](const std::unique_ptr<T> & u_ptr){return u_ptr.get();}
    );
    return std::move(raw_pointers);
}

template<typename T>
std::vector<T*> MapToRawPointers(const std::vector<std::shared_ptr<T>> & shared_pointers)  {
    std::vector<T *> raw_pointers(shared_pointers.size());
    std::transform(
            shared_pointers.begin(), shared_pointers.end(),
            raw_pointers.begin(),
            [](const std::shared_ptr<T> & s_ptr) {return s_ptr.get();}
    );
    return std::move(raw_pointers);
}

#endif //PIRATESSIMULATION_UTIL_H
