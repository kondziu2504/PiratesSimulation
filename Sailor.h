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
    std::mutex sailor_mutex;
    Ship * ship;
    bool upper_deck = true;

    //Sailor activity/travel
    std::mutex target_mutex;
    float activity_progress;
    int previous_target = ShipObjectIdGenerator::kNoObject;
    int next_target = ShipObjectIdGenerator::kNoObject;

    //Operated elements
    std::shared_ptr<Mast> operated_mast = nullptr;
    std::shared_ptr<Cannon> operated_cannon = nullptr;

    std::atomic<bool> dying = false;


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
    explicit Sailor(Ship * ship);
    void Start();
    void Kill();
    [[nodiscard]] bool GetIsDying() const;
    SailorState GetState();
    bool IsUpperDeck();

    //Operated elements
    [[nodiscard]] std::shared_ptr<Mast> GetOperatedMast() const;
    [[nodiscard]] std::shared_ptr<Cannon> GetOperatedCannon() const;

    //Sailor activities/travel
    int GetPreviousTarget();
    int GetNextTarget();
    float GetProgress();
};


#endif //PIRATESSIMULATION_SAILOR_H
