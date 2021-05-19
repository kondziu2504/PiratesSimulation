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
#include <unordered_set>
#include <unordered_map>
#include "Sailor.h"

enum class MonitorDisplayMode {kMap, kDashboard};

template<typename T>
using sp = std::shared_ptr<T>;

class Monitor {
    std::shared_ptr<World> world;
    MonitorDisplayMode display_mode = MonitorDisplayMode::kMap;
    std::mutex display_mode_mutex;
    int curr_column = 0, curr_row = 0;
    std::mutex current_ship_mutex;
    int current_ship_ind = 0;

    static const std::vector<int> kSailorsColors;

    static std::unordered_map<Sailor *, int> sailors_assigned_colors;
    static int color_id;

    sp<std::unordered_set<sp<Ship>>> visualized_ships;
    sp<std::unordered_map<void *, Vec2>> elements_positions;

    enum class Tile {kWater = 1, kLand, kShip, kSail, kGray, kSailor, kStairs, kCannon, kCannonball};
public:
    Monitor(std::shared_ptr<World> world);

    void Start();
    void Stop();
private:
    void Update();
    void Initialize();
    void DrawWorld(int x_offset, int y_offset, int x_viewport, int y_viewport, int viewport_width, int viewport_height);
    void DrawShips(int x_offset, int y_offset, int x_viewport, int y_viewport, int viewport_width, int viewport_height);
    void DrawShip(std::shared_ptr<Ship>, int x_offset, int y_offset, int x_viewport, int y_viewport, int viewport_width, int viewport_height);
    void DrawMap(int x_offset, int y_offset, int x_viewport, int y_viewport, int viewport_width, int viewport_height);
    void DrawTile(int y, int x, char ch, Tile tile);
    void DrawDashboard(std::shared_ptr <Ship> ship);
    void DrawShipDeck(std::shared_ptr <Ship> ship, int x_offset, int y_offset, int width, int height);
    void DrawCircleIndicator(int x_offset, int y_offset, float angle, std::string label, int size);
    void DrawWindDir(int x_offset, int y_offset, int size);
    void DrawShipDir(int x_offset, int y_offset, int size, std::shared_ptr <Ship> ship);
    void DrawSailTarget(int x_offset, int y_offset, int size, std::shared_ptr<Ship> ship);
    void DrawShipInfo(int ship_ind);
    void DrawCannonballs(int x_offset, int y_offset, int x_viewport, int y_viewport, int viewport_width, int viewport_height);
    int GetColor(Sailor * sailor);

    void InitColorpairs();
    short CursColor(int fg);
    int ColorNum(int fg, int bg);
    void SetColor(int fg, int bg);
    void UnsetColor(int fg, int bg);

    [[noreturn]] void InputThread();


    [[noreturn]] void UpdateThread();
};


#endif //PIRATESSIMULATION_MONITOR_H
