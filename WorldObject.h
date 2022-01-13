//
// Created by konrad on 01.01.2022.
//

#ifndef PIRATESSIMULATION_WORLDOBJECT_H
#define PIRATESSIMULATION_WORLDOBJECT_H


#include "Util/Vec2f.h"
#include "World.h"

class WorldObject {
    Vec2f position;
    Vec2f direction;
    World * world;
    std::mutex _mutex;

public:
    WorldObject(Vec2f direction, Vec2f position, World * world) : direction(direction), position(position), world(world) {};
    Vec2f GetPosition();
    void SetPosition(Vec2f new_pos);
    Vec2f GetDirection();
    void SetDirection(Vec2f new_dir);
    World * GetWorld();
};


#endif //PIRATESSIMULATION_WORLDOBJECT_H
