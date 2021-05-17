//
// Created by konrad on 14.05.2021.
//

#ifndef PIRATESSIMULATION_MONITOR_H
#define PIRATESSIMULATION_MONITOR_H

#include <memory>
#include "World.h"
#include "Vec2.h"
#include <pthread.h>
#include <ncurses.h>

enum class MonitorDisplayMode {kMap, kDashboard};

class Monitor {
    std::shared_ptr<World> world;
    MonitorDisplayMode display_mode = MonitorDisplayMode::kMap;
    std::mutex display_mode_mutex;
    int curr_column = 0, curr_row = 0;

    enum class Tile {kWater = 1, kLand, kShip, kSail, kGray};
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
    void DrawDashboard();
    void DrawShipDeck(int x_offset, int y_offset, int width, int height);
    void DrawCircleIndicator(int x_offset, int y_offset, float angle, std::string label, int size);
    void DrawWindDir(int x_offset, int y_offset, int size);
    void DrawShipDir(int x_offset, int y_offset, int size, std::shared_ptr <Ship> ship);
    void DrawSailTarget(int x_offset, int y_offset, int size, std::shared_ptr<Ship> ship);

    [[noreturn]] void InputThread();


    [[noreturn]] void UpdateThread();
};


#endif //PIRATESSIMULATION_MONITOR_H
