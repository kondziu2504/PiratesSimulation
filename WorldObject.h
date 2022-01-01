//
// Created by konrad on 01.01.2022.
//

#ifndef PIRATESSIMULATION_WORLDOBJECT_H
#define PIRATESSIMULATION_WORLDOBJECT_H


#include "Vec2.h"
#include "World.h"

class WorldObject {
protected:
    Vec2 position;
    Vec2 direction;
    World * world;

public:
    WorldObject(Vec2 direction, Vec2 position, World * world) : direction(direction), position(position), world(world) {};
    Vec2 GetPosition();
    Vec2 GetDirection();
    World * GetWorld();
};


#endif //PIRATESSIMULATION_WORLDOBJECT_H
