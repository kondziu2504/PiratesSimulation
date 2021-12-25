//
// Created by konrad on 16.05.2021.
//

#ifndef PIRATESSIMULATION_SAILOR_H
#define PIRATESSIMULATION_SAILOR_H

#include <random>
#include <mutex>
#include <atomic>
#include "ShipObjectIdGenerator.h"
#include "ShipObject.h"

class Cannon;
class Ship;
class Mast;

enum class SailorState {kMast, kResting, kWalking, kWaitingMast, kStanding,
        kWaitingStairs, kStairs, kWaitingCannon, kCannon, kDead};

class Sailor : public ShipObject {
    const static std::mt19937 mt;
    const static std::uniform_real_distribution<double> distribution;

    SailorState currentState = SailorState::kResting;

    std::mutex target_mutex;
    float activity_progress;
    bool upper_deck = true;
    Ship * ship;

    int previous_target = ShipObjectIdGenerator::kNoObject;
    int next_target = ShipObjectIdGenerator::kNoObject;

    std::mutex sailor_mutex;



    void ThreadFun();
    void OperateMast();
    void GoOperateMast();
    void WaitForMast();
    void GoWaitForMast();

    void GoRest();
    void SetState(SailorState new_state);
    void SetProgress(float progress);
    void SetPreviousTarget(int target);
    void SetNextTarget(int target);
    void UseStairs();
    void GoUseStairs();
    void GoWaitForCannon();
    void WaitForCannon();
    void GoUseCannon();
    void UseCannon();
    void Walk(int next, float seconds);

public:
    std::atomic<bool> dying = false;
    std::shared_ptr<Mast> operated_mast = nullptr;
    Cannon * operated_cannon = nullptr;
    bool IsUpperDeck();
    int GetPreviousTarget();
    int GetNextTarget();
    float GetProgress();
    explicit Sailor(Ship * ship);
    void Start();
    void Kill();
    SailorState GetState();
};


#endif //PIRATESSIMULATION_SAILOR_H
