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
    mutable std::mutex ships_mutex;
    std::vector<std::shared_ptr<Ship>> ships;

    static std::unique_ptr<bool[]> GenerateMap(int map_width, int map_height, int seed);

    Vec2f FindFreeSpotForShip();
    void ShipLiveAndRespawn(std::shared_ptr<Ship> ship);
    bool respawn = true;


public:
    void Stop();

    std::mutex cannonballs_mutex;
    std::vector<std::shared_ptr<Cannonball>> cannonballs;
    std::vector<std::shared_ptr<Ship>> GetShips() const;

    std::shared_ptr<Wind> wind;
    World(int width, int height, int seed);
    void AddShip(const std::shared_ptr<Ship>& ship);
    void AddRandomShip();
    [[nodiscard]] int GetWidth() const;
    [[nodiscard]] int GetHeight() const;


    [[nodiscard]] bool TileInsideWorld(int x, int y) const;
    [[nodiscard]] bool IsLandAt(Vec2i coords) const;


    bool CorrectCoords(Vec2i coords) const;
};


#endif //PIRATESSIMULATION_WORLD_H
