//
// Created by konrad on 01.01.2022.
//

#include "ShipController.h"
#include <memory>
#include <mutex>
#include "../WorldObject.h"
#include "Ship.h"

using namespace std;

void ShipController::EngageFight(Ship *enemy_ship) {
    if(enemy_ship->GetState() == ShipState::kFighting)
        return;
    PrepareForFight(enemy_ship);
    enemy_ship->PrepareForFight(parent);
}

void ShipController::PrepareForFight(Ship *ship) {
    enemy = ship;
    SetState(ShipState::kFighting);
    crew->SetOrders(SailorOrder::kOperateCannons);
    crew->SetCannonsTarget(ship);
}

bool ShipController::LookForEnemy() {
    parent->GetPosition();
    lock_guard<mutex> guard(parent->GetWorld()->ships_mutex);
    for(auto ship : parent->GetWorld()->ships){
        if(ship.get() != parent &&
           ship->GetState() != ShipState::kSinking &&
           ship->GetState() != ShipState::kDestroyed){
            const int lookout_radius = 9;
            float dist = (ship->GetPosition() - parent->GetPosition()).Length();
            if(dist < lookout_radius){
                EngageFight(ship.get());
                return true;
            }
        }
    }
    return false;
}

void ShipController::GetInPosition() {
    float fighting_angle = DetermineAngleToFaceEnemy();
    StartTurningTowardsAngle(fighting_angle);
}

void ShipController::StartTurningTowardsAngle(float target_angle) {
    auto CurrentAngle = [&]() -> float {return parent->GetDirection().Angle();};
    const float kMaxDeviationFromTargetAngle = M_PI / 180.f * 5.f;
    auto CurrentDeviation = [&]() -> float {return abs(CurrentAngle() - target_angle);};

    while(CurrentDeviation() > kMaxDeviationFromTargetAngle){
        float angle_diff = AngleDifference(target_angle, CurrentAngle());
        const float kAngleCorrection = min(abs(angle_diff), 0.1f) * (angle_diff > 0 ? 1 : -1);
        parent->SetDirection(parent->GetDirection().Rotated(kAngleCorrection));
        SleepSeconds(0.1f);
    }
}

float ShipController::DetermineAngleToFaceEnemy() {
    Vec2 to_enemy = enemy->GetPosition() - parent->GetPosition();
    Vec2 dir_with_cannons_on_left = to_enemy.Rotated(M_PI_2);
    Vec2 dir_with_cannons_on_right = to_enemy.Rotated(-M_PI_2);
    float target_angle;
    float to_cannons_left_required_rotation = abs(AngleDifference(parent->GetDirection().Angle(), dir_with_cannons_on_left.Angle()));
    float to_cannons_right_required_rotation = abs(AngleDifference(parent->GetDirection().Angle(), dir_with_cannons_on_right.Angle()));
    if(to_cannons_left_required_rotation < to_cannons_right_required_rotation) {
        target_angle = dir_with_cannons_on_left.Angle();
        crew->SetUseRightCannons(false);
    } else {
        target_angle = dir_with_cannons_on_right.Angle();
        crew->SetUseRightCannons(true);
    }
    return target_angle;
}

void ShipController::SetState(ShipState new_state) {
    state = new_state;
}

ShipState ShipController::GetState() {
    return state;
}

void ShipController::AdjustDirection() {
    const int scan_dist = 9;
    int closest_tile_dist = scan_dist;
    Vec2 total_correction(0, 0);
    total_correction += CalculateCorrectionAgainstLand(scan_dist, closest_tile_dist);
    total_correction += CalculateCorrectionAgainstShips(scan_dist, closest_tile_dist);
    ApplyCorrection(scan_dist, closest_tile_dist, total_correction);
}

Vec2 ShipController::CalculateCorrectionAgainstLand(int scan_dist, int &closest_tile_dist) const {
    Vec2 correction(0, 0);
    for(int j = 0; j < 16; j++){
        Vec2 checkDir = Vec2::FromAngle((float)j/16 * 2 * M_PI);
        for(int i = 0; i < scan_dist; i++){
            Vec2 scannedTile = parent->GetPosition() + checkDir * i;
            scannedTile.x = (int)scannedTile.x;
            scannedTile.y = (int)scannedTile.y;
            bool tileOutsideWorld = scannedTile.x < 0 || scannedTile.x >= parent->GetWorld()->width ||
                                    scannedTile.y < 0 || scannedTile.y > parent->GetWorld()->height;
            if(parent->GetWorld()->map[scannedTile.y * parent->GetWorld()->width + scannedTile.x] || tileOutsideWorld){
                if(!(parent->GetPosition() - scannedTile).Length() == 0){
                    Vec2 correction_dir = (parent->GetPosition() - scannedTile).Normalized();
                    float significance = (float)(scan_dist - i) / scan_dist;
                    closest_tile_dist = min(closest_tile_dist, i);
                    correction = correction + correction_dir * significance;
                }
            }
        }
    }
    return correction;
}

Vec2 ShipController::CalculateCorrectionAgainstShips(int scan_dist, int &closest_tile_dist) const {
    Vec2 correction;
    lock_guard<mutex> guard(parent->GetWorld()->ships_mutex);
    for(shared_ptr<Ship> ship : parent->GetWorld()->ships){
        if(ship.get() != parent){
            Vec2 ship_pos = ship->GetPosition();
            float dist = (parent->GetPosition() - ship_pos).Length();
            if(dist < scan_dist && dist > 0){
                Vec2 correction_dir = (parent->GetPosition()  - ship_pos).Normalized();
                float significance = (float)(scan_dist - dist) / scan_dist;
                closest_tile_dist = min(closest_tile_dist, (int)dist);
                correction = correction + correction_dir * significance;
            }
        }
    }
    return correction;
}

void ShipController::ApplyCorrection(int scan_dist, int closest_tile_dist, Vec2 correction) {
    if(correction.Length() != 0){
        Vec2 normalized_correction = correction.Normalized();
        float correction_significance = (float)(scan_dist - closest_tile_dist) / scan_dist;
        if(parent->GetDirection().Dot(normalized_correction) > 0)
            correction_significance = 0;
        parent->SetDirection(parent->GetDirection() + normalized_correction * correction_significance);
        parent->SetDirection(parent->GetDirection().Normalized());
    }
}

Ship *ShipController::GetEnemy() {
    return enemy;
}

ShipController::ShipController(ShipBody *ship_body, Crew *crew, Ship *parent) {
    this->ship_body = ship_body;
    this->crew = crew;
    this->parent = parent;
}

void ShipController::Start() {
    crew->SetOrders(SailorOrder::kOperateMasts);
    while(true){
        ShipState current_state = GetState();
        if(current_state == ShipState::kDestroyed)
            return;
        if(current_state == ShipState::kWandering){
            LookForEnemy();
            AdjustDirection();
            SleepSeconds(0.1f);
        }else if(current_state == ShipState::kFighting){
            GetInPosition();
            if(enemy->GetState() == ShipState::kSinking || enemy->GetState() == ShipState::kDestroyed) {
                enemy = nullptr;
                SetState(ShipState::kWandering);
                crew->SetOrders(SailorOrder::kOperateMasts);
                crew->SetCannonsTarget(nullptr);
            }
        }
        if(kill){
            SetState(ShipState::kDestroyed);
            return;
        }
    }
}

void ShipController::Kill() {
    kill = true;
    SetState(ShipState::kSinking);
}
