//
// Created by konrad on 14.05.2021.
//

#include "World.h"
#include <memory>
#include "PerlinNoise.hpp"
#include "Ship/Ship.h"
#include "Wind.h"
#include <thread>
#include "Ship/Cannonball.h"

using namespace std;

World::World(int width, int height, int seed) :
        width(width),
        height(height),
        map(GenerateMap(width, height, seed)),
        wind(make_shared<Wind>(this)) {
}

std::unique_ptr<bool[]> World::GenerateMap(int map_width, int map_height, unsigned int seed) {
    auto generated_map = make_unique<bool[]>(map_width * map_height);
    siv::PerlinNoise perlinNoise(seed);
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
            generated_map[ind] = noiseVal > landThreshold;
        }
    }
    return generated_map;
}

void World::AddShip(const std::shared_ptr<Ship>& ship) {
    lock_guard<mutex> guard(ships_mutex);
    ships.push_back(ship);
    ShipLiveAndRespawn(ship);
}

void World::AddRandomShip() {
    Vec2f pos = FindFreeSpotForShip();
    Vec2f direction = Vec2f::FromAngle((float)rand() / RAND_MAX * 6.28f);
    int sailors = 10 + rand() % 25;
    int masts = 2 + rand() % 3;
    int cannonsPerSide = 2 + rand() % 3;

    auto newShip = make_shared<Ship>(pos, direction, sailors, masts, cannonsPerSide, this);
    AddShip(newShip);
}

void World::ShipLiveAndRespawn(const shared_ptr<Ship>& ship) {
    thread ship_thread([=](){
        ship->Start();
        ship->WaitUntilStopped();
        {
            lock_guard<mutex> guard(ships_mutex);
            ships.erase(std::remove(ships.begin(), ships.end(), ship));
        }
        if(respawn)
            AddRandomShip();
    });
    ship_thread.detach();
}

Vec2f World::FindFreeSpotForShip() {
    const int searched_square_size = 14;
    const int half_square = searched_square_size / 2;
    Vec2f pos;
    bool good_field;
    do{
        good_field = true;
        pos = Vec2f((float)(rand() % width), (float)(rand() % height));
        for(int x = -half_square; x <= half_square / 2; x++) {
            for(int y = -half_square; y <= half_square; y++) {
                Vec2f new_pos = pos + Vec2f((float)x, (float)y);
                if (new_pos.x < 0 || new_pos.x >= (float)width || new_pos.y < 0 || new_pos.y >= (float)height) {
                    good_field = false;
                    break;
                }
                if(IsLandAt((Vec2i)new_pos)) {
                    good_field = false;
                    break;
                }
            }
            if (!good_field)
                break;
        }
        {
            lock_guard<mutex> guard(ships_mutex);
            for(const auto& ship : ships){
                if((ship->GetPosition() - pos).Length() < half_square)
                    good_field = false;
            }
        }
    }while(!good_field);

    return pos;
}

bool World::CoordsInsideWorld(Vec2i coords) const {
    return coords.x >= 0 && coords.x < width
           && coords.y >= 0 && coords.y < height;
}

bool World::IsLandAt(Vec2i coords) const {
    return map[coords.y * width + coords.x];
}

int World::GetWidth() const {
    return width;
}

int World::GetHeight() const {
    return height;
}

vector<std::shared_ptr<Ship>> World::GetShips() const {
    lock_guard<mutex> guard(ships_mutex);
    return ships;
}

std::vector<std::shared_ptr<Cannonball>> World::GetCannonballs() const {
    lock_guard<mutex> guard(cannonballs_mutex);
    return cannonballs;
}

void World::AddCannonball(const std::shared_ptr<Cannonball>& cannonball) {
    {
        lock_guard<mutex> guard(cannonballs_mutex);
        cannonballs.push_back(cannonball);
    }
    thread cannonball_thread([=](){
        cannonball->Start();
        cannonball->WaitUntilStopped();
        {
            lock_guard<mutex> guard(cannonballs_mutex);
            cannonballs.erase(std::remove(cannonballs.begin(), cannonballs.end(), cannonball));
        }
    });
    cannonball_thread.detach();
}

std::shared_ptr<Wind> World::GetWind() const {
    return wind;
}

void World::ThreadFunc(const atomic<bool> &stop_requested) {
    wind->Start();

    WaitUntilStopRequested();

    wind->RequestStop();

    respawn = false;
    auto ships_copy = ships;
    for (const auto &ship: ships_copy)
        ship->RequestStop();
    for (const auto &ship: ships_copy)
        ship->WaitUntilStopped();

    wind->WaitUntilStopped();
}
