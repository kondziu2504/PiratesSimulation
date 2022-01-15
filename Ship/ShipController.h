//
// Created by konrad on 01.01.2022.
//

#ifndef PIRATESSIMULATION_SHIPCONTROLLER_H
#define PIRATESSIMULATION_SHIPCONTROLLER_H

#include "ShipBody.h"
#include "Crew.h"
#include "ShipState.h"
#include <atomic>

class Ship;

class ShipController : public Stoppable {
    Crew * crew;

    std::atomic<ShipState> state = ShipState::kWandering;
    Ship * parent = nullptr;
    Ship * enemy = nullptr;
    const float enemy_lookout_radius = 9.f;
    const float obstacles_lookout_radius = 9.f;

    void ThreadFunc(const std::atomic<bool> &stop_requested) override;

    // Fighting
    void EngageFight(Ship * enemy_ship);

    bool LookForEnemy();
    void GetInPosition();
    void StartTurningTowardsAngle(float target_angle);
    float DetermineAngleToFaceEnemy();

    void SetState(ShipState new_state);
    void AdjustDirection();

    Vec2f CalculateCorrectionAgainstLand(float & closest_tile_dist) const;
    Vec2f CalculateCorrectionAgainstShips(float & closest_tile_dist) const;
    void ApplyCorrection(float closest_tile_dist, Vec2f correction);

public:
    explicit ShipController(Crew *crew, Ship *parent);
    ShipState GetState();
    void PrepareForFight(Ship * ship);
};


#endif //PIRATESSIMULATION_SHIPCONTROLLER_H
