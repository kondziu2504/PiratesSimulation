//
// Created by konrad on 14.05.2021.
//

#include "World.h"
#include <memory>
#include "PerlinNoise.hpp"
#include "Ship.h"
#include "Wind.h"
#include <thread>

using namespace std;

World::World(int width, int height, int seed) {
    this->map = shared_ptr<bool[]>(new bool[width * height]);
    this->width = width;
    this->height = height;
    this->wind = make_shared<Wind>(this);

    wind->Start();
    GenerateMap(width, height, seed);
}

void World::GenerateMap(int map_width, int map_height, int seed) {
    siv::PerlinNoise perlinNoise((double)seed);
    float noiseFrequency = 0.03;
    float landThreshold = 0.6f;

    for(int x = 0; x < map_width; x++){
        for(int y = 0; y < map_height; y++){
            float noiseVal = (float)perlinNoise.normalizedOctaveNoise2D_0_1(
                    (float)x * noiseFrequency,
                    (float)y * noiseFrequency,
                    1);
            noiseVal = 1 - noiseVal;
            int ind = y * map_width + x;
            map[ind] = noiseVal > landThreshold;
        }
    }
}

void World::AddShip(const std::shared_ptr<Ship>& ship) {
    lock_guard<mutex> guard(ships_mutex);
    ships.push_back(ship);
    ShipLiveAndRespawn(ship);
}

void World::AddRandomShip() {
    Vec2 pos = FindFreeSpotForShip();
    Vec2 direction = Vec2::FromAngle((float)rand() / RAND_MAX * 6.28);
    int sailors = 10 + rand() % 25;
    int masts = 2 + rand() % 3;
    int cannonsPerSide = 2 + rand() % 3;

    auto newShip = make_shared<Ship>(pos, direction, sailors, masts, cannonsPerSide, this);
    AddShip(newShip);
}

void World::Stop() {
    respawn = false;
    for (auto ship_r_it = ships.rbegin(); ship_r_it != ships.rend(); ++ship_r_it) {
        (*ship_r_it)->Destroy();
    }
    while(true) {
        {
            lock_guard<mutex> guard(ships_mutex);
            if (ships.empty())
                break;
        }
        SleepSeconds(0.1f);
    }
}

bool World::TileInsideWorld(int x, int y) const {
    return x < 0 || x >= width || y < 0 || y > height;
}

bool World::IsLandAt(int x, int y) const {
    return map[y * width + x];
}

void World::ShipLiveAndRespawn(shared_ptr<Ship> ship) {
    thread ship_thread([=](){
        ship->Start();
        {
            lock_guard<mutex> guard(ships_mutex);
            ships.erase(std::remove(ships.begin(), ships.end(), ship));
        }
        if(respawn)
            AddRandomShip();
    });
    ship_thread.detach();
}

Vec2 World::FindFreeSpotForShip() {
    const int searched_square_size = 14;
    const int half_square = searched_square_size / 2;
    Vec2 pos;
    bool goodField;
    do{
        goodField = true;
        pos = Vec2(rand() % width, rand() % height);
        for(int x = -half_square; x <= half_square / 2; x++) {
            for(int y = -half_square; y <= half_square; y++) {
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
        {
            lock_guard<mutex> guard(ships_mutex);
            for(const auto& ship : ships){
                if((ship->GetPosition() - pos).Length() < half_square)
                    goodField = false;
            }
        }
    }while(!goodField);

    return pos;
}
