#include "WorldObject.h"

using namespace std;

Vec2 WorldObject::GetPosition() {
    return position;
}

Vec2 WorldObject::GetDirection() {
    return direction;
}

World *WorldObject::GetWorld() {
    return world;
}

void WorldObject::SetPosition(Vec2 new_pos) {
    lock_guard<mutex> guard(_mutex);
    position = new_pos;
}

void WorldObject::SetDirection(Vec2 new_dir) {
    lock_guard<mutex> guard(_mutex);
    direction = new_dir;
}
