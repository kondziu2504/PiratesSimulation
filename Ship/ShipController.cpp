//
// Created by konrad on 01.01.2022.
//

#include "ShipController.h"
#include <memory>
#include <mutex>
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
    for(const auto& ship : parent->GetWorld()->GetShips()){
        if(ship != parent &&
           ship->GetState() != ShipState::kSinking &&
           ship->GetState() != ShipState::kDestroyed){
            float dist = (ship->GetPosition() - parent->GetPosition()).Length();
            if(dist < enemy_lookout_radius){
                EngageFight(ship);
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
        const float kAngleCorrection = min(abs(angle_diff), 0.1f) * (angle_diff > 0 ? 1.f : -1.f);
        parent->SetDirection(parent->GetDirection().Rotated(kAngleCorrection));
        SleepSeconds(0.1f);
    }
}

float ShipController::DetermineAngleToFaceEnemy() {
    Vec2f to_enemy = enemy->GetPosition() - parent->GetPosition();
    Vec2f dir_with_cannons_on_left = to_enemy.Rotated(M_PI_2);
    Vec2f dir_with_cannons_on_right = to_enemy.Rotated(-M_PI_2);
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
    float closest_tile_dist = obstacles_lookout_radius;
    Vec2f total_correction(0, 0);
    total_correction += CalculateCorrectionAgainstLand(closest_tile_dist);
    total_correction += CalculateCorrectionAgainstShips(closest_tile_dist);
    ApplyCorrection(closest_tile_dist, total_correction);
}

Vec2f ShipController::CalculateCorrectionAgainstLand(float & closest_tile_dist) const {
    Vec2f correction(0, 0);
    for(int j = 0; j < 16; j++){
        Vec2f checkDir = Vec2f::FromAngle((float)j/16 * 2 * (float)M_PI);
        for(int i = 0; i < (int)obstacles_lookout_radius; i++){
            Vec2i scannedTile = (Vec2i)(parent->GetPosition() + checkDir * i);
            bool tileOutsideWorld = !parent->GetWorld()->CoordsInsideWorld(scannedTile);
            if(parent->GetWorld()->IsLandAt(scannedTile) || tileOutsideWorld){
                if((parent->GetPosition() - (Vec2f)scannedTile).Length() != 0){
                    Vec2f correction_dir = (parent->GetPosition() - (Vec2f)scannedTile).Normalized();
                    float significance = (obstacles_lookout_radius - (float)i) / obstacles_lookout_radius;
                    closest_tile_dist = min(closest_tile_dist, (float)i);
                    correction = correction + correction_dir * significance;
                }
            }
        }
    }
    return correction;
}

Vec2f ShipController::CalculateCorrectionAgainstShips(float &closest_tile_dist) const {
    Vec2f correction;
    for(const auto ship : parent->GetWorld()->GetShips()){
        if(ship != parent){
            Vec2f ship_pos = ship->GetPosition();
            float dist = (parent->GetPosition() - ship_pos).Length();
            if(dist < obstacles_lookout_radius && dist > 0){
                Vec2f correction_dir = (parent->GetPosition()  - ship_pos).Normalized();
                float significance = (float)(obstacles_lookout_radius - dist) / obstacles_lookout_radius;
                closest_tile_dist = min(closest_tile_dist, dist);
                correction = correction + correction_dir * significance;
            }
        }
    }
    return correction;
}

void ShipController::ApplyCorrection(float closest_tile_dist, Vec2f correction) {
    if(correction.Length() != 0){
        Vec2f normalized_correction = correction.Normalized();
        float correction_significance = (float)(obstacles_lookout_radius - closest_tile_dist) / obstacles_lookout_radius;
        if(parent->GetDirection().Dot(normalized_correction) > 0)
            correction_significance = 0;
        parent->SetDirection(parent->GetDirection() + normalized_correction * correction_significance);
        parent->SetDirection(parent->GetDirection().Normalized());
    }
}

ShipController::ShipController(Crew *crew, Ship *parent) {
    this->crew = crew;
    this->parent = parent;
}

void ShipController::ThreadFunc(const atomic<bool> &stop_requested) {
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
        if(stop_requested){
            SetState(ShipState::kDestroyed);
            return;
        }
    }
}