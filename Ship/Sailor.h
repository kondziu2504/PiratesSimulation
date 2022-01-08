//
// Created by konrad on 16.05.2021.
//

#ifndef PIRATESSIMULATION_SAILOR_H
#define PIRATESSIMULATION_SAILOR_H

#include <random>
#include <mutex>
#include <atomic>
#include "ShipObject.h"
#include "../Util/Util.h"
#include "../Util/Vec2.h"
#include "ShipBody.h"

class Cannon;
class Ship;
class Mast;

enum class SailorActionStatus {
    kSuccess,
    kKilledDuringAction
};

enum class SailorState {kMast, kResting, kWalking, kWaitingMast, kStanding,
        kWaitingStairs, kStairs, kWaitingCannon, kCannon, kDead};

enum class SailorOrder {kOperateCannons, kOperateMasts};

class Sailor {
    SailorState currentState = SailorState::kResting;
    std::mutex sailor_mutex;
    ShipBody * ship;
    WorldObject * parent;

    std::atomic<bool> upper_deck = true;

    SailorOrder current_order = SailorOrder::kOperateMasts;
    WorldObject * cannon_target = nullptr;
    std::atomic<bool> use_right_cannons = false;

    //Sailor activity/travel
    std::mutex target_mutex;
    float activity_progress;
    std::shared_ptr<ShipObject> previous_target = nullptr;
    std::shared_ptr<ShipObject> next_target = nullptr;

    //Operated elements
    std::shared_ptr<Mast> operated_mast = nullptr;
    std::shared_ptr<Cannon> assigned_cannon = nullptr;

    std::atomic<bool> dying = false;

    SailorActionStatus GoTo(std::shared_ptr<ShipObject> shipObject);

    SailorActionStatus OperateTheShip();

    [[nodiscard]] std::shared_ptr<ShipObject> GetFightingSideJunction() const;
    [[nodiscard]] std::vector<std::shared_ptr<Cannon>> GetFightingSideCannons() const;
    Vec2 CalculateCannonTarget() const;
    SailorActionStatus FulfillAssignedCannonRole();
    bool TryClaimFirstUnoccupiedCannon();

    //Mast operations
    SailorActionStatus OperateMast();
    void WaitForMast();
    SailorActionStatus GoUseMastProcedure();
    SailorActionStatus ContinuouslyAdjustMast();
    void ReleaseMast();

    SailorActionStatus WaitForCannon();
    SailorActionStatus UseCannon();
    SailorActionStatus GoUseCannonProcedure();

    SailorActionStatus GoRest();
    SailorActionStatus ProgressAction(float actionTotalTime, std::function<void(float progress)> action = nullptr);

    void SetState(SailorState new_state);
    void SetProgress(float progress);

    SailorActionStatus SleepAndCheckKilled(float seconds);

public:
    explicit Sailor(ShipBody *ship_body, WorldObject *parent);
    void Start();
    void Kill();
    [[nodiscard]] bool GetIsDying() const;
    SailorState GetState();
    void SetCurrentOrder(SailorOrder new_order);
    void SetCannonTarget(WorldObject * cannon_target);
    void SetUseRightCannons(bool right);

    //Operated elements
    [[nodiscard]] std::shared_ptr<Mast> GetOperatedMast() const;
    [[nodiscard]] std::shared_ptr<Cannon> GetOperatedCannon() const;

    //Sailor activities/travel
    std::shared_ptr<ShipObject> GetPreviousTarget();
    std::shared_ptr<ShipObject> GetNextTarget();
    float GetProgress();
};


#endif //PIRATESSIMULATION_SAILOR_H
