//
// Created by konrad on 14.05.2021.
//

#ifndef PIRATESSIMULATION_MONITOR_H
#define PIRATESSIMULATION_MONITOR_H

#include <memory>
#include "World.h"
#include "Util/Vec2f.h"
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

class Monitor : public Stopable{
    enum class Tile {kWater = 1, kLand, kShip, kSail, kGray, kSailor, kCannon, kCannonball, kDestroyed, kIndicator};

    s_ptr<World> world;
    std::atomic<MonitorDisplayMode> display_mode = MonitorDisplayMode::kMap;
    std::atomic<int> current_ship_ind = 0;

    static const std::vector<int> kSailorsColors;
    static const std::unordered_map<SailorState, std::string> sailor_states_map_strings;
    static const std::unordered_map<Tile, int> tiles_map_color_pairs;

    void ThreadFunc(const std::atomic<bool> &stop_requested) override;

    static void DrawTile(Vec2i screen_coords, char character, Tile tile);
    static int GetSailorColor(int sailor_index);
    static void InitializeNcurses();
    static int GetTileColorPair(Tile tile);

    void Update();

    void DrawShipInfo(const std::shared_ptr<Ship>& ship);
    void DrawChosenShip();

    void DrawWorld(Vec2i screen_offset = Vec2i(0, 0), Rect world_viewport = Rect(0, 0, 0, 0));
    void DrawMap(Vec2i screen_offset, Rect world_viewport);
    void DrawShips(Vec2i screen_offset, Rect world_viewport);
    void DrawShip(const std::shared_ptr<Ship>&, Vec2i screen_offset, Rect world_viewport);
    void DrawCannonballs(Vec2i screen_offset, Rect world_viewport);

    void DrawDashboard(std::shared_ptr <Ship> ship);
    void DrawDashboardCannons(const std::vector<std::shared_ptr<Cannon>>& cannons, ncurses_util::ConsoleWriter & console_writer) const;
    void DrawDashboardCannons(const std::shared_ptr<Ship>& ship, ncurses_util::ConsoleWriter & console_writer) const;
    void DrawDashboardMasts(std::shared_ptr<Ship> &ship, ncurses_util::ConsoleWriter &console_writer) const;
    void DrawDashboardSailors(std::shared_ptr<Ship> &ship, ncurses_util::ConsoleWriter &console_writer);

    void DrawShipDeck(std::shared_ptr <Ship> ship, Rect screen_rect);
    s_ptr<std::unordered_map<s_ptr<ShipObject>, Vec2i>>  GenerateElementsPositions(s_ptr<Ship> &ship, const Rect &screen_rect) const;
    void DrawShipDeckFloor(const Rect &screen_rect) const;
    void DrawShipDeckCannons(const s_ptr<Ship>& ship, Rect screen_rect, const s_ptr<std::unordered_map<s_ptr<ShipObject>, Vec2i>>& elements_positions);
    void DrawShipDeckMasts(const s_ptr<Ship>& ship, Rect screen_rect, const s_ptr<std::unordered_map<s_ptr<ShipObject>, Vec2i>>& elements_positions);
    void DrawShipDeckSailors(const s_ptr<Ship>& ship, Rect screen_rect, const s_ptr<std::unordered_map<s_ptr<ShipObject>, Vec2i>>& elements_positions);

    void DrawCircleIndicator(Vec2i screen_coords, float arrow_angle, int diameter, const std::string &label,
                             const std::string &sub_label = "");
    void DrawWindDirIndicator(Vec2i offset, int size);
    void DrawShipDirIndicator(Vec2i offset, int size, const std::shared_ptr <Ship>& ship);
    void DrawSailTargetDirIndicator(Vec2i offset, int size, const std::shared_ptr<Ship>& ship);

public:
    explicit Monitor(std::shared_ptr<World> world);

    void NextShip();
    void PrevShip();
    void ChangeDisplayMode();
};


#endif //PIRATESSIMULATION_MONITOR_H
