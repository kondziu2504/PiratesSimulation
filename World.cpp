//
// Created by konrad on 14.05.2021.
//

#include "World.h"
#include <memory>

using namespace std;

World::World(int width, int height) {
    this->map = make_shared<bool[]>(width * height);
    this->width = width;
    this->height = height;
}

void World::AddShip(std::shared_ptr<Ship> ship) {
    lock_guard<mutex> guard(shipsMutex);
    ships.push_back(ship);
}
