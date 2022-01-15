#include "WorldObject.h"

using namespace std;

Vec2f WorldObject::GetPosition() const {
    lock_guard<mutex> guard(_mutex);
    return position;
}

Vec2f WorldObject::GetDirection() const {
    lock_guard<mutex> guard(_mutex);
    return direction;
}

World *WorldObject::GetWorld() const {
    return world;
}

void WorldObject::SetPosition(Vec2f new_pos) {
    lock_guard<mutex> guard(_mutex);
    position = new_pos;
}

void WorldObject::SetDirection(Vec2f new_dir) {
    lock_guard<mutex> guard(_mutex);
    direction = new_dir;
}
