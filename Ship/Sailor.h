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
#include "../Util/Vec2f.h"
#include "ShipBody.h"

class Cannon;
class Ship;
class Mast;

enum class SailorActionStatus {
    kSuccess,
    kKilledDuringAction
};

enum class SailorState {kMast, kResting, kWalking, kWaitingMast, kStanding,
        kWaitingCannon, kCannon, kDead};

enum class SailorOrder {kOperateCannons, kOperateMasts};

class Sailor : public Stoppable {
    SailorState currentState = SailorState::kResting;
    mutable std::mutex sailor_mutex;
    ShipBody * const ship;
    WorldObject * const parent;

    void ThreadFunc(const std::atomic<bool> &stop_requested) override;

    SailorOrder current_order = SailorOrder::kOperateMasts;
    WorldObject * cannon_target = nullptr;
    std::atomic<bool> use_right_cannons = false;

    //Sailor activity/travel
    mutable std::mutex target_mutex;
    float activity_progress = 0.f;
    ShipObject * previous_target = nullptr;
    ShipObject * next_target = nullptr;

    //Operated elements
    Mast * operated_mast = nullptr;
    Cannon * assigned_cannon = nullptr;

    SailorActionStatus GoTo(ShipObject * shipObject);

    SailorActionStatus OperateTheShip();

    ShipObject * GetFightingSideJunction() const;
    std::vector<Cannon *> GetFightingSideCannons() const;
    Vec2f CalculateCannonTarget() const;
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
    SailorState GetState() const;
    void SetCurrentOrder(SailorOrder new_order);
    void SetCannonTarget(WorldObject * cannon_target);
    void SetUseRightCannons(bool right);

    //Operated elements
    Mast * GetOperatedMast() const;
    Cannon * GetOperatedCannon() const;

    //Sailor activities/travel
    ShipObject * GetPreviousTarget() const;
    ShipObject * GetNextTarget() const;
    float GetProgress() const;
};


#endif //PIRATESSIMULATION_SAILOR_H
