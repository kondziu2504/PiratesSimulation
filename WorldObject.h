//
// Created by konrad on 01.01.2022.
//

#ifndef PIRATESSIMULATION_WORLDOBJECT_H
#define PIRATESSIMULATION_WORLDOBJECT_H


#include "Util/Vec2.h"
#include "World.h"

class WorldObject {
    Vec2 position;
    Vec2 direction;
    World * world;
    std::mutex _mutex;

public:
    WorldObject(Vec2 direction, Vec2 position, World * world) : direction(direction), position(position), world(world) {};
    Vec2 GetPosition();
    void SetPosition(Vec2 new_pos);
    Vec2 GetDirection();
    void SetDirection(Vec2 new_dir);
    World * GetWorld();
};


#endif //PIRATESSIMULATION_WORLDOBJECT_H
