//
// Created by konrad on 14.05.2021.
//

#ifndef PIRATESSIMULATION_WORLD_H
#define PIRATESSIMULATION_WORLD_H

#include <memory>
#include <vector>
#include <mutex>
#include "Util/Vec2f.h"
#include "Util/Vec2i.h"

class Ship;
class Wind;
class Cannonball;

class World {
private:
    const int width, height;
    const std::unique_ptr<const bool[]> map;

    std::vector<std::shared_ptr<Ship>> ships;
    mutable std::mutex ships_mutex;

    std::vector<std::shared_ptr<Cannonball>> cannonballs;
    mutable std::mutex cannonballs_mutex;

    const std::shared_ptr<Wind> wind;
    bool respawn = true;

    static std::unique_ptr<bool[]> GenerateMap(int map_width, int map_height, unsigned int seed);
    Vec2f FindFreeSpotForShip();
    void ShipLiveAndRespawn(std::shared_ptr<Ship> ship);

public:
    World(int width, int height, int seed);

    void Start();
    void Stop();

    std::vector<std::shared_ptr<Ship>> GetShips() const;
    void AddShip(const std::shared_ptr<Ship>& ship);
    void AddRandomShip();
    std::vector<std::shared_ptr<Cannonball>> GetCannonballs() const;
    void AddCannonball(std::shared_ptr<Cannonball> cannonball);
    std::shared_ptr<Wind> GetWind() const;
    int GetWidth() const;
    int GetHeight() const;
    bool IsLandAt(Vec2i coords) const;
    bool CoordsInsideWorld(Vec2i coords) const;
};


#endif //PIRATESSIMULATION_WORLD_H
