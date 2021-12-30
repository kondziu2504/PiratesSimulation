#include <thread>
#include <utility>
#include "Sailor.h"
#include "Mast.h"
#include "Ship.h"
#include "Wind.h"
#include "World.h"
#include "MastDistributor.h"
#include "Util.h"
#include "Cannon.h"

using namespace std;

void Sailor::Start() {
    thread sailor_thread(&Sailor::ThreadFun, this);
    sailor_thread.detach();
}

void Sailor::ThreadFun() {
    SleepSeconds(RandomTime(0,10));
    while(true){
        OperateTheShip();
        if (dying) {
            SetState(SailorState::kDead);
            return;
        }
        GoRest();
    }
}


void Sailor::GoRest() {
    GoTo(ship->GetRestingPoint());
    SetState(SailorState::kResting);
    SleepSeconds(RandomTime(3,5));
    SetState(SailorState::kStanding);
}

Sailor::Sailor(Ship * ship) {
    this->ship = ship;
}

SailorState Sailor::GetState() {
    lock_guard<mutex> guard(sailor_mutex);
    return currentState;
}

void Sailor::SetState(SailorState new_state) {
    lock_guard<mutex> guard(sailor_mutex);
    currentState = new_state;
}

void Sailor::OperateMast() {
    SetState(SailorState::kMast);
    ContinuouslyAdjustMast();
    ReleaseMast();
    SetState(SailorState::kStanding);
}

void Sailor::WaitForMast() {
    SetState(SailorState::kWaitingMast);
    operated_mast = ship->GetMastDistributor()->RequestMast(this);
    SetState(SailorState::kStanding);
}

float Sailor::GetProgress() {
    lock_guard<mutex> guard(sailor_mutex);
    return activity_progress;
}

void Sailor::SetProgress(float progress) {
    lock_guard<mutex> guard(sailor_mutex);
    activity_progress = progress;
}

std::shared_ptr<ShipObject> Sailor::GetPreviousTarget() {
    lock_guard<mutex> guard(target_mutex);
    return previous_target;
}

std::shared_ptr<ShipObject> Sailor::GetNextTarget() {
    lock_guard<mutex> guard(target_mutex);
    return next_target;
}

void Sailor::UseStairs() {
    SetState(SailorState::kWaitingStairs);
    lock_guard<mutex> guard(ship->GetStairs()->mutex);
    SetState(SailorState::kStairs);
    ProgressAction(2.f);
    upper_deck = !upper_deck;
    SetState(SailorState::kStanding);
}

void Sailor::GoUseStairs() {
    SetState(SailorState::kWalking);
    next_target = ship->GetStairs();
    ProgressAction(2.f);
    previous_target = next_target;
    SetState(SailorState::kStanding);

    UseStairs();
}

bool Sailor::IsUpperDeck() {
    lock_guard<mutex> guard(sailor_mutex);
    return upper_deck;
}

void Sailor::WaitForCannon() {
    SetState(SailorState::kWaitingCannon);
    while(true){
        if(TryClaimFirstUnoccupiedCannon())
            return;
        else
            SleepSeconds(0.1f);
    }
}

void Sailor::UseCannon() {
    SetState(SailorState::kCannon);
    SleepSeconds(2);
    FulfillAssignedCannonRole();
    assigned_cannon->Release(this);
    SetState(SailorState::kStanding);
}

void Sailor::GoTo(shared_ptr<ShipObject> shipObject) {
    SetState(SailorState::kWalking);
    next_target = std::move(shipObject);
    ProgressAction(3.f);
    previous_target = next_target;
    SetState(SailorState::kStanding);
}

void Sailor::Kill(){
    dying = true;
    while(GetState() != SailorState::kDead);
}

std::shared_ptr<Mast> Sailor::GetOperatedMast() const {
    return operated_mast;
}

std::shared_ptr<Cannon> Sailor::GetOperatedCannon() const {
    return assigned_cannon;
}

bool Sailor::GetIsDying() const {
    return dying;
}

std::shared_ptr<ShipObject> Sailor::GetFightingSideJunction() const {
    return ship->GetUseRightCannons() ? ship->GetRightJunction() : ship->GetLeftJunction();
}

void Sailor::GoUseMastProcedure() {
    GoTo(ship->GetRightJunction());
    WaitForMast();
    GoTo(operated_mast);
    OperateMast();
}

void Sailor::GoUseCannonProcedure() {
    GoTo(GetFightingSideJunction());
    WaitForCannon();
    GoTo(assigned_cannon);
    UseCannon();
}

void Sailor::ContinuouslyAdjustMast() {
    const float workTime = 6.f;
    ProgressAction(workTime,
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

void Sailor::OperateTheShip() {
    ShipState ship_state = ship->GetState();
    if(ship_state == ShipState::kWandering) {
        GoUseMastProcedure();
    }else if(ship_state == ShipState::kFighting){
        GoUseCannonProcedure();
    }
}

void Sailor::ProgressAction(float actionTotalTime, std::function<void(float progress)> action) {
    DoRepeatedlyForATime(
            [=](float progress) -> void
            {
                if(action != nullptr)
                    action(progress);
                SetProgress(progress);
            },
            actionTotalTime
    );
}

std::vector<std::shared_ptr<Cannon>> Sailor::GetFightingSideCannons() const {
    return ship->GetUseRightCannons() ? ship->GetRightCannons() : ship->GetLeftCannons();
}

Vec2 Sailor::CalculateCannonTarget() const {
    float distance = 5;
    if(ship->GetEnemy() != nullptr)
        distance = (ship->GetEnemy()->GetPos() - ship->GetPos()).Length();
    Vec2 perpendicular_right = Vec2::FromAngle(ship->GetDir().Angle() + M_PI_2).Normalized() * distance;
    return ship->GetPos() + perpendicular_right * (ship->GetUseRightCannons() ? 1.f : -1.f);
}

void Sailor::FulfillAssignedCannonRole() {
    auto cannon_owners = assigned_cannon->GetOwners();
    if(cannon_owners.first == this){
        assigned_cannon->WaitUntilLoadedOrTimeout();
        if(assigned_cannon->Loaded())
            assigned_cannon->Shoot(CalculateCannonTarget());
    }else if(cannon_owners.second == this){
        assigned_cannon->Load();
    }
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



