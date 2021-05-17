//
// Created by konrad on 16.05.2021.
//

#ifndef PIRATESSIMULATION_SAILOR_H
#define PIRATESSIMULATION_SAILOR_H

#include <random>
#include <mutex>
class Ship;
class Mast;

enum class SailorState {kMast, kResting, kWalking, kWaitingMast, kStanding};

class Sailor {
    std::mt19937 mt;
    std::uniform_real_distribution<double> distribution = std::uniform_real_distribution(4.0, 6.0);

    SailorState currentState = SailorState::kResting;

    std::mutex target_mutex;

    float activity_progress;

    Ship * ship;
    std::shared_ptr<Mast> operated_mast;
    std::shared_ptr<void> previous_target = nullptr;
    std::shared_ptr<void> next_target = nullptr;

    std::mutex sailor_mutex;

    [[noreturn]] void ThreadFun();
    void OperateMast();
    void GoOperateMast();
    void WaitForMast();
    void GoWaitForMast();
    void GoRest();
    void SetState(SailorState new_state);
    void SetProgress(float progress);
    void SetPreviousTarget(std::shared_ptr<void> target);
    void SetNextTarget(std::shared_ptr<void> target);
public:
    std::shared_ptr<void> GetPreviousTarget();
    std::shared_ptr<void> GetNextTarget();
    float GetProgress();
    explicit Sailor(Ship * ship);
    void Start();
    SailorState GetState();
};


#endif //PIRATESSIMULATION_SAILOR_H
