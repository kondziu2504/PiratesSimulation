//
// Created by konrad on 14.05.2021.
//

#include "World.h"
#include <memory>
#include "PerlinNoise.hpp"
#include "Ship.h"
#include "Wind.h"

using namespace std;

World::World(int width, int height, int seed) {
    this->map = shared_ptr<bool[]>(new bool[width * height]);
    this->width = width;
    this->height = height;
    this->wind = make_shared<Wind>(this);
    wind->Start();

    siv::PerlinNoise perlinNoise((double)seed);
    float noiseFrequency = 0.03;
    float landThreshold = 0.6f;
    int border = 1;

    for(int x = 0; x < width; x++){
        for(int y = 0; y < height; y++){
            float noiseVal = perlinNoise.normalizedOctaveNoise2D_0_1(
                    (float)x * noiseFrequency,
                    (float)y * noiseFrequency,
                    1);
            noiseVal = 1 - noiseVal;
            int ind = y * width + x;
            map[ind] = noiseVal > landThreshold;

        }
    }
}

void World::AddShip(std::shared_ptr<Ship> ship) {
    lock_guard<mutex> guard(shipsMutex);
    ships.push_back(ship);
    ship->Start();
}
