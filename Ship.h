//
// Created by konrad on 14.05.2021.
//

#ifndef PIRATESSIMULATION_SHIP_H
#define PIRATESSIMULATION_SHIP_H


#include "Vec2.h"
#include <mutex>
#include <memory>
#include <vector>
#include "ShipObject.h"
#include "Stairs.h"
#include <memory>
#include <atomic>

class MastDistributor;
class Mast;
class World;
class Sailor;
class Cannon;

enum class ShipState{kWandering, kFighting, kSinking, kDestroyed};

class Ship {
    // Ship status
    Vec2 pos;
    std::mutex pos_mutex;
    Vec2 direction;
    std::atomic<ShipState> state = ShipState::kWandering;
    int hp = 10;

    World * world;
    bool use_right_cannons = true;
    float length = 6;
    Ship * enemy = nullptr;
    const std::shared_ptr<const std::vector<std::shared_ptr<Sailor>>> sailors;
    std::shared_ptr<MastDistributor> distributor;

    // Ship objects
    std::shared_ptr<ShipObject> left_junction;
    std::shared_ptr<ShipObject> right_junction;
    std::vector<std::shared_ptr<Cannon>> left_cannons;
    std::vector<std::shared_ptr<Cannon>> right_cannons;
    std::shared_ptr<Stairs> stairs;
    std::shared_ptr<ShipObject> resting_point;
    std::shared_ptr<std::vector<std::shared_ptr<Mast>>> masts;

    std::shared_ptr<std::vector<std::shared_ptr<Sailor>>> GenerateSailors(int sailors_count);

    // Fighting
    void EngageFight(Ship * ship);
    void PrepareForFight(Ship * ship);
    bool LookForEnemy();
    void GetInPosition();
    void StartTurningTowardsAngle(float target_angle);
    float DetermineAngleToFaceEnemy();

    void SetState(ShipState new_state);
    void AdjustDirection();

    void UpdateThread();

    Vec2 CalculateCorrectionAgainstLand(int scan_dist, int& closest_tile_dist) const;
    Vec2 CalculateCorrectionAgainstShips(int scan_dist, int& closest_tile_dist) const;
    void ApplyCorrection(int scan_dist, int closest_tile_dist, Vec2 correction);

public:
    Ship(Vec2 pos, Vec2 direction, int sailors, int masts, int cannons_per_side, World * world);

    void Start();
    void Destroy(bool respawn);

    void ApplyWind(Vec2 wind);
    Vec2 GetPos();
    Vec2 GetDir();
    [[nodiscard]] int GetHP() const;
    void Hit(int damage);
    [[nodiscard]] bool GetUseRightCannons() const;
    [[nodiscard]] float GetLength() const;
    ShipState GetState();
    Ship * GetEnemy();
    World * GetWorld();
    std::vector<std::shared_ptr<Sailor>> GetSailors();
    std::shared_ptr<MastDistributor> GetMastDistributor();

    // Ship objects
    std::shared_ptr<ShipObject> GetLeftJunction();
    std::shared_ptr<ShipObject> GetRightJunction();
    std::vector<std::shared_ptr<Cannon>> GetLeftCannons();
    std::vector<std::shared_ptr<Cannon>> GetRightCannons();
    std::shared_ptr<Stairs> GetStairs();
    std::shared_ptr<ShipObject> GetRestingPoint();
    std::vector<std::shared_ptr<Mast>> GetMasts();
};


#endif //PIRATESSIMULATION_SHIP_H
