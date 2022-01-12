//
// Created by konrad on 14.05.2021.
//

#ifndef PIRATESSIMULATION_MONITOR_H
#define PIRATESSIMULATION_MONITOR_H

#include <memory>
#include "World.h"
#include "Util/Vec2.h"
#include <pthread.h>
#include <ncurses.h>
#include <unordered_set>
#include <unordered_map>
#include "Ship/Sailor.h"
#include "Util/Rect.h"
#include <atomic>
#include "Util/NcursesUtil.h"

enum class MonitorDisplayMode {kMap, kDashboard};

template<typename T>
using s_ptr = std::shared_ptr<T>;

class Monitor {
    s_ptr<World> world;
    std::atomic<MonitorDisplayMode> display_mode = MonitorDisplayMode::kMap;
    std::atomic<int> current_ship_ind = 0;

    static const std::vector<int> kSailorsColors;

    static std::unordered_map<Sailor *, int> sailors_assigned_colors;
    static int color_id;

    enum class Tile {kWater = 1, kLand, kShip, kSail, kGray, kSailor, kStairs, kCannon, kCannonball, kDestroyed, kIndicator};

    std::unordered_map<SailorState, std::string> sailor_states_map_strings = {
            {SailorState::kResting, "Resting"},
            {SailorState::kMast, "Operating mast"},
            {SailorState::kWalking, "Walking"},
            {SailorState::kCannon, "Operating cannon"},
            {SailorState::kWaitingCannon, "Waiting for cannon"},
            {SailorState::kStanding, "Standing"},
            {SailorState::kWaitingMast, "Waiting for mast"},
            {SailorState::kDead, "Dead"}
    };

    std::atomic<bool> stop = false;



    void Update();
    void Initialize();

    void DrawMap(Vec2i screen_offset, Rect world_viewport);
    void DrawWorld(Vec2i screen_offset = Vec2i(0, 0), Rect world_viewport = Rect(0, 0, 0, 0));
    void DrawShips(Vec2i screen_offset, Rect world_viewport);
    void DrawShip(std::shared_ptr<Ship>, Vec2i screen_offset, Rect world_viewport);
    static void DrawTile(Vec2i screen_coords, char character, Tile tile);
    void DrawDashboard(std::shared_ptr <Ship> ship);
    void DrawShipDeck(std::shared_ptr <Ship> ship, Rect screen_rect);
    void DrawCircleIndicator(Vec2i screen_coords, float angle, std::string label, int diameter);
    void DrawWindDirIndicator(Vec2i offset, int size);
    void DrawShipDirIndicator(Vec2i offset, int size, std::shared_ptr <Ship> ship);
    void DrawSailTargetDirIndicator(Vec2i offset, int size, std::shared_ptr<Ship> ship);
    void DrawShipInfo(std::shared_ptr<Ship> ship);
    void DrawCannonballs(Vec2i screen_offset, Rect world_viewport);
    int GetColor(Sailor * sailor);

    void DrawChosenShip();
    void DrawDashboardCannons(std::vector<std::shared_ptr<Cannon>> cannons, ncurses_util::ConsoleWriter & console_writer) const;
    void DrawDashboardCannons(std::shared_ptr<Ship> ship, ncurses_util::ConsoleWriter & console_writer) const;
    void DrawShipDeckFloor(const Rect &screen_rect) const;
    void DrawDashboardMasts(std::shared_ptr<Ship> &ship, ncurses_util::ConsoleWriter &console_writer) const;
    void DrawDashboardSailors(std::shared_ptr<Ship> &ship, ncurses_util::ConsoleWriter &console_writer);
    void DrawShipDeckCannons(s_ptr<Ship> ship, Rect screen_rect, s_ptr<std::unordered_map<s_ptr<ShipObject>, Vec2>> elements_positions);
    void DrawShipDeckMasts(s_ptr<Ship> ship, Rect screen_rect, s_ptr<std::unordered_map<s_ptr<ShipObject>, Vec2>> elements_positions);
    void DrawShipDeckSailors(s_ptr<Ship> ship, Rect screen_rect, s_ptr<std::unordered_map<s_ptr<ShipObject>, Vec2>> elements_positions);
    s_ptr<std::unordered_map<s_ptr<ShipObject>, Vec2>>  GenerateElementsPositions(s_ptr<Ship> &ship, const Rect &screen_rect) const;

public:
    explicit Monitor(std::shared_ptr<World> world);

    void Start();
    void Stop();

    void NextShip();
    void PrevShip();
    void ChangeDisplayMode();


};


#endif //PIRATESSIMULATION_MONITOR_H
