//
// Created by konrad on 16.05.2021.
//

#ifndef PIRATESSIMULATION_SAILOR_H
#define PIRATESSIMULATION_SAILOR_H

#include <random>
class Ship;

enum class SailorState {kMast, kResting};

class Sailor {
    std::mt19937 mt;
    std::uniform_real_distribution<double> distribution = std::uniform_real_distribution(4.0, 6.0);

    SailorState currentState = SailorState::kResting;
    Ship * ship;

    [[noreturn]] void ThreadFun();
    void GoOperateMast();
    void GoRest();
public:
    explicit Sailor(Ship * ship);
    void Start();
};


#endif //PIRATESSIMULATION_SAILOR_H
