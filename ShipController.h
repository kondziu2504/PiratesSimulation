//
// Created by konrad on 01.01.2022.
//

#ifndef PIRATESSIMULATION_SHIPCONTROLLER_H
#define PIRATESSIMULATION_SHIPCONTROLLER_H

#include "ShipBody.h"
#include "Crew.h"
#include "ShipState.h"

class Ship;

class ShipController {
    ShipBody * ship_body;
    Crew * crew;

    std::atomic<ShipState> state = ShipState::kWandering;
    bool use_right_cannons = true;
    Ship * parent = nullptr;
    Ship * enemy = nullptr;

    bool kill = false;

    // Fighting
    void EngageFight(Ship * enemy_ship);

    bool LookForEnemy();
    void GetInPosition();
    void StartTurningTowardsAngle(float target_angle);
    float DetermineAngleToFaceEnemy();
    Ship * GetEnemy();
    [[nodiscard]] bool GetUseRightCannons() const;

    void SetState(ShipState new_state);
    void AdjustDirection();

    Vec2 CalculateCorrectionAgainstLand(int scan_dist, int& closest_tile_dist) const;
    Vec2 CalculateCorrectionAgainstShips(int scan_dist, int& closest_tile_dist) const;
    void ApplyCorrection(int scan_dist, int closest_tile_dist, Vec2 correction);

public:
    explicit ShipController(ShipBody *ship_body, Crew *crew, Ship *parent);
    ShipState GetState();
    void PrepareForFight(Ship * ship);
    void Start();
    void Kill();
};


#endif //PIRATESSIMULATION_SHIPCONTROLLER_H
