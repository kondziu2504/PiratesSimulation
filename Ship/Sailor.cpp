#include <thread>
#include <utility>
#include "Sailor.h"
#include "Mast.h"
#include "Ship.h"
#include "MastDistributor.h"
#include "Cannon.h"

using namespace std;

SailorActionStatus Sailor::GoRest() {
    GoTo(ship->GetRestingPoint());
    SetState(SailorState::kResting);
    if(SleepAndCheckKilled(RandomFromRange(3, 5)) == SailorActionStatus::kKilledDuringAction)
        return SailorActionStatus::kKilledDuringAction;
    SetState(SailorState::kStanding);
    return SailorActionStatus::kSuccess;
}

Sailor::Sailor(ShipBody *ship_body, WorldObject *parent) :
        ship(ship_body), parent(parent) {

}

SailorState Sailor::GetState() const {
    lock_guard<mutex> guard(sailor_mutex);
    return currentState;
}

void Sailor::SetState(SailorState new_state) {
    lock_guard<mutex> guard(sailor_mutex);
    currentState = new_state;
}

SailorActionStatus Sailor::OperateMast() {
    SetState(SailorState::kMast);
    if(ContinuouslyAdjustMast() == SailorActionStatus::kKilledDuringAction){
        ReleaseMast();
        return SailorActionStatus::kKilledDuringAction;
    }
    ReleaseMast();
    SetState(SailorState::kStanding);
    return SailorActionStatus::kSuccess;
}

void Sailor::WaitForMast() {
    SetState(SailorState::kWaitingMast);
    operated_mast = ship->GetMastDistributor()->RequestMast(this);
    SetState(SailorState::kStanding);
}

float Sailor::GetProgress() const {
    lock_guard<mutex> guard(sailor_mutex);
    return activity_progress;
}

void Sailor::SetProgress(float progress) {
    lock_guard<mutex> guard(sailor_mutex);
    activity_progress = progress;
}

ShipObject * Sailor::GetPreviousTarget() const {
    lock_guard<mutex> guard(target_mutex);
    return previous_target;
}

ShipObject * Sailor::GetNextTarget() const {
    lock_guard<mutex> guard(target_mutex);
    return next_target;
}

SailorActionStatus Sailor::WaitForCannon() {
    SetState(SailorState::kWaitingCannon);
    while(true){
        if(TryClaimFirstUnoccupiedCannon())
            return SailorActionStatus::kSuccess;
        else
            if(SleepAndCheckKilled(0.1f) == SailorActionStatus::kKilledDuringAction)
                return SailorActionStatus::kKilledDuringAction;
    }
}

SailorActionStatus Sailor::UseCannon() {
    SetState(SailorState::kCannon);
    if(SleepAndCheckKilled(2) == SailorActionStatus::kKilledDuringAction){
        assigned_cannon->Release(this);
        return SailorActionStatus::kKilledDuringAction;
    }
    FulfillAssignedCannonRole();
    assigned_cannon->Release(this);
    SetState(SailorState::kStanding);
    return SailorActionStatus::kSuccess;
}

SailorActionStatus Sailor::GoTo(ShipObject * shipObject) {
    SetState(SailorState::kWalking);
    next_target = shipObject;
    if(ProgressAction(3.f) == SailorActionStatus::kKilledDuringAction)
        return SailorActionStatus::kKilledDuringAction;
    previous_target = next_target;
    SetState(SailorState::kStanding);
    return SailorActionStatus::kSuccess;
}

Mast * Sailor::GetOperatedMast() const {
    return operated_mast;
}

Cannon * Sailor::GetOperatedCannon() const {
    return assigned_cannon;
}

ShipObject * Sailor::GetFightingSideJunction() const {
    return use_right_cannons ? ship->GetRightJunction() : ship->GetLeftJunction();
}

SailorActionStatus Sailor::GoUseMastProcedure() {
    if(GoTo(ship->GetRightJunction()) == SailorActionStatus::kKilledDuringAction)
        return SailorActionStatus::kKilledDuringAction;
    WaitForMast();
    if(GoTo(operated_mast) == SailorActionStatus::kKilledDuringAction)
        return SailorActionStatus::kKilledDuringAction;
    OperateMast();
    return SailorActionStatus::kSuccess;
}

SailorActionStatus Sailor::GoUseCannonProcedure() {
    if(GoTo(GetFightingSideJunction()) == SailorActionStatus::kKilledDuringAction)
        return SailorActionStatus::kKilledDuringAction;
    if(WaitForCannon() == SailorActionStatus::kKilledDuringAction)
        return SailorActionStatus::kKilledDuringAction;
    if(GoTo(assigned_cannon) == SailorActionStatus::kKilledDuringAction)
        return SailorActionStatus::kKilledDuringAction;
    if(UseCannon() == SailorActionStatus::kKilledDuringAction)
        return SailorActionStatus::kKilledDuringAction;
    return SailorActionStatus::kSuccess;
}

SailorActionStatus Sailor::ContinuouslyAdjustMast() {
    const float workTime = 6.f;
    return ProgressAction(workTime,
                   [=](float progress) -> void {
                       operated_mast->Adjust();
                   });
}

void Sailor::ReleaseMast() {
    ship->GetMastDistributor()->ReleaseMast(operated_mast, this);
    {
        lock_guard<mutex> guard(sailor_mutex);
        operated_mast = nullptr;
    }
}

SailorActionStatus Sailor::OperateTheShip() {
    if(current_order == SailorOrder::kOperateMasts) {
        return GoUseMastProcedure();
    }else if(current_order == SailorOrder::kOperateCannons){
        return GoUseCannonProcedure();
    }
    return SailorActionStatus::kSuccess;
}

SailorActionStatus Sailor::ProgressAction(float actionTotalTime, std::function<void(float progress)> action) {
    SailorActionStatus action_status = SailorActionStatus::kSuccess;
    DoRepeatedlyForATime(
            [&](float progress, bool & stop) -> void
            {
                if(action != nullptr)
                    action(progress);
                SetProgress(progress);
                if(GetStopRequested()){
                    action_status = SailorActionStatus::kKilledDuringAction;
                    stop = true;
                }
            },
            actionTotalTime
    );
    return action_status;
}

std::vector<Cannon *> Sailor::GetFightingSideCannons() const {
    return use_right_cannons ? ship->GetRightCannons() : ship->GetLeftCannons();
}

Vec2f Sailor::CalculateCannonTarget() const {
    float distance = 5;
    auto _cannon_target = cannon_target.lock();
    if(_cannon_target)
        distance = (_cannon_target->GetPosition() - parent->GetPosition()).Length();
    Vec2f perpendicular_right = Vec2f::FromAngle(parent->GetDirection().Angle() + (float)M_PI_2).Normalized() * distance;
    return parent->GetPosition() + perpendicular_right * (use_right_cannons ? 1.f : -1.f);
}

SailorActionStatus Sailor::FulfillAssignedCannonRole() {
    auto cannon_owners = assigned_cannon->GetOwners();
    if(cannon_owners.first == this){
        assigned_cannon->WaitUntilLoadedOrTimeout();
        if(GetStopRequested())
            return SailorActionStatus::kKilledDuringAction;
        if(assigned_cannon->Loaded())
            assigned_cannon->Shoot(CalculateCannonTarget());
    }else if(cannon_owners.second == this){
        assigned_cannon->Load();
    }
    return SailorActionStatus::kSuccess;
}

bool Sailor::TryClaimFirstUnoccupiedCannon() {
    auto side_cannons = GetFightingSideCannons();
    for(const auto& cannon : side_cannons){
        if(cannon->TryClaim(this)){
            assigned_cannon = cannon;
            SetState(SailorState::kStanding);
            return true;
        }
    }
    return false;
}

void Sailor::SetCurrentOrder(SailorOrder new_order) {
    current_order = new_order;
}

void Sailor::SetCannonTarget(const weak_ptr<WorldObject> & target) {
    this->cannon_target = cannon_target;
}

SailorActionStatus Sailor::SleepAndCheckKilled(float seconds) {
    const float time_step = 0.1f;
    float seconds_left = seconds;
    while(seconds_left > 0){
        SleepSeconds(time_step);
        seconds_left -= time_step;
        if(GetStopRequested())
            return SailorActionStatus::kKilledDuringAction;
    }
    return SailorActionStatus::kSuccess;
}

void Sailor::SetUseRightCannons(bool right) {
    use_right_cannons = right;
}

void Sailor::ThreadFunc(const atomic<bool> &stop_requested) {
    if(SleepAndCheckKilled(RandomFromRange(0, 10)) == SailorActionStatus::kKilledDuringAction){
        SetState(SailorState::kDead);
        return;
    }
    while(true){
        if(OperateTheShip() == SailorActionStatus::kKilledDuringAction)
            break;
        if(GoRest() == SailorActionStatus::kKilledDuringAction)
            break;
    }
    SetState(SailorState::kDead);
}



