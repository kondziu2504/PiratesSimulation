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
    Crew * const crew;

    std::atomic<ShipState> state = ShipState::kWandering;
    Ship * const parent = nullptr;
    std::weak_ptr<Ship> enemy;
    const float enemy_lookout_radius = 9.f;
    const float obstacles_lookout_radius = 9.f;

    void ThreadFunc(const std::atomic<bool> &stop_requested) override;

    // Fighting
    void TryEngageFight(const std::weak_ptr<Ship> & enemy_ship);

    bool LookForEnemy();
    void GetInPosition();
    void StartTurningTowardsAngle(float target_angle);
    float DetermineAngleToFaceEnemy(const std::shared_ptr<Ship> & enemy);

    void SetState(ShipState new_state);
    void AdjustDirection();

    Vec2f CalculateCorrectionAgainstLand(float & closest_tile_dist) const;
    Vec2f CalculateCorrectionAgainstShips(float & closest_tile_dist) const;
    void ApplyCorrection(float closest_tile_dist, Vec2f correction);

public:
    explicit ShipController(Crew *crew, Ship *parent);
    ShipState GetState() const;
    void PrepareForFight(const std::weak_ptr<Ship>& ship);
};


#endif //PIRATESSIMULATION_SHIPCONTROLLER_H
