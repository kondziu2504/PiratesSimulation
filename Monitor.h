//
// Created by konrad on 14.05.2021.
//

#ifndef PIRATESSIMULATION_MONITOR_H
#define PIRATESSIMULATION_MONITOR_H

#include <memory>
#include "World.h"
#include <pthread.h>
#include <ncurses.h>

class Monitor {
    std::shared_ptr<World> world;

    int curr_column = 0, curr_row = 0;

    enum class Tile {kWater = 1, kLand, kShip};
public:
    Monitor(std::shared_ptr<World> world);

    void Start();
    void Stop();
private:
    void Update();
    void Initialize();
    void DrawWorld();
    void DrawShips();
    void DrawShip(std::shared_ptr<Ship>);
    void DrawMap();
    void DrawTile(int y, int x, char ch, Tile tile);


    [[noreturn]] void UpdateThread();
};


#endif //PIRATESSIMULATION_MONITOR_H
