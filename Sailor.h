//
// Created by konrad on 16.05.2021.
//

#ifndef PIRATESSIMULATION_SAILOR_H
#define PIRATESSIMULATION_SAILOR_H

#include <random>
#include <mutex>

class Cannon;
class Ship;
class Mast;

enum class SailorState {kMast, kResting, kWalking, kWaitingMast, kStanding,
        kWaitingStairs, kStairs, kWaitingCannon, kCannon};

class Sailor {
    std::mt19937 mt;
    std::uniform_real_distribution<double> distribution = std::uniform_real_distribution(4.0, 6.0);

    SailorState currentState = SailorState::kResting;

    std::mutex target_mutex;

    float activity_progress;

    bool upper_deck = true;

    Ship * ship;
    std::shared_ptr<Mast> operated_mast;
    Cannon * operated_cannon = nullptr;
    void * previous_target = nullptr;
    void * next_target = nullptr;

    std::mutex sailor_mutex;

    [[noreturn]] void ThreadFun();
    void OperateMast();
    void GoOperateMast();
    void WaitForMast();
    void GoWaitForMast();

    void GoRest();
    void SetState(SailorState new_state);
    void SetProgress(float progress);
    void SetPreviousTarget(void * target);
    void SetNextTarget(void * target);
    void UseStairs();
    void GoUseStairs();
    void GoWaitForCannon();
    void WaitForCannon();
    void GoUseCannon();
    void UseCannon();
    void Walk(void * next, float seconds);
public:
    bool IsUpperDeck();
    void * GetPreviousTarget();
    void * GetNextTarget();
    float GetProgress();
    explicit Sailor(Ship * ship);
    void Start();
    SailorState GetState();
};


#endif //PIRATESSIMULATION_SAILOR_H
