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

void World::GenerateShip() {
    Vec2 pos;
    bool goodField;
    do{
        goodField = true;
        pos = Vec2(rand() % width, rand() % height);
        for(int x = - 10; x <= 10; x++) {
            for(int y = -10; y <= 10; y++) {
                Vec2 newPos = Vec2(pos.x + x, pos.y + y);
                if (newPos.x < 0 || newPos.x >= width || newPos.y < 0 || newPos.y >= height) {
                    goodField = false;
                    break;
                }
                if(map[newPos.y * width + newPos.x]) {
                    goodField = false;
                    break;
                }
            }
            if (!goodField)
                break;
        }
    }while(!goodField);

    Vec2 direction = Vec2::FromAngle((float)rand() / RAND_MAX * 6.28);
    int sailors = 10 + rand() % 25;
    int masts = 2 + rand() % 3;
    int cannonsPerSide = 2 + rand() % 3;

    lock_guard<mutex> guard(shipsMutex);
    auto newShip = make_shared<Ship>(pos, direction, sailors, masts, cannonsPerSide, this);
    ships.push_back(newShip);
    newShip->Start();
}

void World::Stop() {
    {
        lock_guard<mutex> guard(shipsMutex);
        for (auto ship : ships) {
            ship->Destroy(false);
        }
    }
    while(true) {
        lock_guard<mutex> guard(shipsMutex);
        if(ships.empty())
            break;
    }
}
