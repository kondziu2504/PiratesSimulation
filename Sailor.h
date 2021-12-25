//
// Created by konrad on 16.05.2021.
//

#ifndef PIRATESSIMULATION_SAILOR_H
#define PIRATESSIMULATION_SAILOR_H

#include <random>
#include <mutex>
#include <atomic>
#include "ShipObject.h"

class Cannon;
class Ship;
class Mast;

enum class SailorState {kMast, kResting, kWalking, kWaitingMast, kStanding,
        kWaitingStairs, kStairs, kWaitingCannon, kCannon, kDead};

class Sailor {
    const static std::mt19937 mt;
    const static std::uniform_real_distribution<double> distribution;

    SailorState currentState = SailorState::kResting;
    std::mutex sailor_mutex;
    Ship * ship;
    bool upper_deck = true;

    //Sailor activity/travel
    std::mutex target_mutex;
    float activity_progress;
    std::shared_ptr<ShipObject> previous_target = nullptr;
    std::shared_ptr<ShipObject> next_target = nullptr;

    //Operated elements
    std::shared_ptr<Mast> operated_mast = nullptr;
    std::shared_ptr<Cannon> assigned_cannon = nullptr;

    std::atomic<bool> dying = false;

    void ThreadFun();

    void GoTo(std::shared_ptr<ShipObject> shipObject);
    [[nodiscard]] std::shared_ptr<ShipObject> GetFightingSideJunction() const;

    void OperateMast();
    void WaitForMast();
    void GoUseMastProcedure();

    void WaitForCannon();
    void UseCannon();
    void GoUseCannonProcedure();

    void GoRest();

    void SetState(SailorState new_state);
    void SetProgress(float progress);

    void UseStairs();
    void GoUseStairs();

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
    std::shared_ptr<ShipObject> GetPreviousTarget();
    std::shared_ptr<ShipObject> GetNextTarget();
    float GetProgress();
};


#endif //PIRATESSIMULATION_SAILOR_H
