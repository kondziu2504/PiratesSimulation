//
// Created by konrad on 14.05.2021.
//

#ifndef PIRATESSIMULATION_WORLD_H
#define PIRATESSIMULATION_WORLD_H

#include <memory>
#include <vector>
#include <mutex>
class Ship;
class Wind;
class Cannonball;

class World {
public:
    int width, height;
    std::shared_ptr<bool[]> map;
    std::vector<std::shared_ptr<Ship>> ships;
    std::mutex shipsMutex;

public:
    std::mutex cannonballs_mutex;
    std::vector<std::shared_ptr<Cannonball>> cannonballs;
    std::shared_ptr<Wind> wind;
    World(int width, int height, int seed);
    void AddShip(std::shared_ptr<Ship> ship);
    void GenerateShip();
    void Stop();
};


#endif //PIRATESSIMULATION_WORLD_H
