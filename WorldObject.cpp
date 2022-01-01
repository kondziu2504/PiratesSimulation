#include "WorldObject.h"

Vec2 WorldObject::GetPosition() {
    return position;
}

Vec2 WorldObject::GetDirection() {
    return direction;
}

World *WorldObject::GetWorld() {
    return world;
}
