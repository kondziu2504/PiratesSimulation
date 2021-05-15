//
// Created by konrad on 14.05.2021.
//

#include <unistd.h>
#include <thread>
#include <iostream>
#include <cmath>
#include "Ship.h"
#include "World.h"
using namespace std;

Ship::Ship(Vec2 pos, shared_ptr<World> world){
    this->pos = pos;
    this->world = world;
    direction = Vec2(5, 2).Normalized();
    velocity = 0.5f;
}

Vec2 Ship::GetPos() {
    lock_guard<mutex> guard(pos_mutex);
    return pos;
}

void Ship::Start() {
    thread shipThread(&Ship::UpdateThread, this);
    shipThread.detach();
}

[[noreturn]] void Ship::UpdateThread() {
    while(true){
        {
            lock_guard<mutex> guard(pos_mutex);
            pos = pos + direction * velocity;
            AdjustDirection();
        }
        usleep(100000);
    }
}

Vec2 Ship::GetDir() {
    lock_guard<mutex> guard(pos_mutex);
    return direction;
}

void Ship::AdjustDirection() {
    Vec2 totalCorrection(0, 0);
    int scan_dist = 9;
    int closest_tile = scan_dist;
    for(int j = 0; j < 16; j++){
        Vec2 checkDir = Vec2::FromAngle((float)j/16 * 2 * M_PI);
        for(int i = 0; i < scan_dist; i++){
            Vec2 scannedTile = pos + checkDir * i;
            scannedTile.x = (int)scannedTile.x;
            scannedTile.y = (int)scannedTile.y;
            bool tileOutsideWorld = scannedTile.x < 0 || scannedTile.x >= world->width ||
                    scannedTile.y < 0 || scannedTile.y > world->height;
            if(world->map[scannedTile.y * world->width + scannedTile.x] || tileOutsideWorld){
                Vec2 correction_dir = (pos - scannedTile).Normalized();
                float significance = (float)(scan_dist - i) / scan_dist;
                closest_tile = min(closest_tile, i);
                totalCorrection = totalCorrection + correction_dir * significance;
            }
        }
    }
    if(totalCorrection.Distance() != 0){
        Vec2 normalizedCorrection = totalCorrection.Normalized();
        float correctionSignificance = (float)(scan_dist - closest_tile) / scan_dist;
        if(direction.Dot(normalizedCorrection) > 0)
            correctionSignificance = 0;
        direction = direction + normalizedCorrection * correctionSignificance;
        direction = direction.Normalized();
    }
}
