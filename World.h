//
// Created by konrad on 14.05.2021.
//

#ifndef PIRATESSIMULATION_WORLD_H
#define PIRATESSIMULATION_WORLD_H

#include <memory>
#include <vector>
#include <mutex>
#include "Vec2.h"

class Ship;
class Wind;
class Cannonball;

class World {
public:
    int width, height;
    std::shared_ptr<bool[]> map;
    std::vector<std::shared_ptr<Ship>> ships;
    std::mutex ships_mutex;
    Vec2 FindFreeSpotForShip();
    void ShipLiveAndRespawn(std::shared_ptr<Ship> ship);
    bool respawn = true;

public:
    std::mutex cannonballs_mutex;
    std::vector<std::shared_ptr<Cannonball>> cannonballs;
    std::shared_ptr<Wind> wind;
    World(int width, int height, int seed);
    void AddShip(const std::shared_ptr<Ship>& ship);
    void AddRandomShip();

    void Stop();
    [[nodiscard]] bool TileInsideWorld(int x, int y) const;
    [[nodiscard]] bool IsLandAt(int x, int y) const;

    void GenerateMap(int map_width, int map_height, int seed);
};


#endif //PIRATESSIMULATION_WORLD_H
