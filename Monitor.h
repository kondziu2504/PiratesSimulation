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

class Monitor : public Stoppable{
    enum class Tile {kWater = 1, kLand, kShip, kSail, kGray, kSailor, kCannon, kCannonball, kDestroyed, kIndicator};

    World * const world;
    std::atomic<MonitorDisplayMode> display_mode = MonitorDisplayMode::kMap;
    std::mutex current_ship_mutex;
    std::weak_ptr<Ship> current_ship;

    std::mutex camera_pos_mutex;
    Vec2i camera_pos = {0, 0};
    const int kCameraMoveDelta = 10;

    static const std::vector<int> kSailorsColors;
    static const std::unordered_map<SailorState, std::string> sailor_states_map_strings;
    static const std::unordered_map<Tile, int> tiles_map_color_pairs;

    void ThreadFunc(const std::atomic<bool> &stop_requested) override;

    static void DrawTile(Vec2i screen_coords, char character, Tile tile);
    static int GetSailorColor(int sailor_index);
    static void InitializeNcurses();
    static int GetTileColorPair(Tile tile);

    void Update();

    void DrawShipInfo(const std::shared_ptr<const Ship> & ship);
    void DrawChosenShip();

    void DrawWorld(Vec2i screen_offset = Vec2i(0, 0), Rect world_viewport = Rect(0, 0, 0, 0));
    void DrawMap(Vec2i screen_offset, Rect world_viewport);
    void DrawShips(Vec2i screen_offset, Rect world_viewport);
    void DrawShip(const std::shared_ptr<const Ship> & ship, Vec2i screen_offset, Rect world_viewport);
    void DrawCannonballs(Vec2i screen_offset, Rect world_viewport);

    void DrawDashboard(const std::shared_ptr<const Ship> & ship);
    void DrawDashboardCannons(const std::vector<Cannon *>& cannons, ncurses_util::ConsoleWriter & console_writer) const;
    void DrawDashboardCannons(const std::shared_ptr<const Ship> & ship, ncurses_util::ConsoleWriter & console_writer) const;
    void DrawDashboardMasts(const std::shared_ptr<const Ship> & ship, ncurses_util::ConsoleWriter &console_writer) const;
    void DrawDashboardSailors(const std::shared_ptr<const Ship> & ship, ncurses_util::ConsoleWriter &console_writer);

    void DrawShipDeck(const std::shared_ptr<const Ship> & ship, Rect screen_rect);
    s_ptr<std::unordered_map<ShipObject *, Vec2i>>  GenerateElementsPositions(const std::shared_ptr<const Ship> & ship, const Rect &screen_rect) const;
    void GenerateMastsPositions(const std::shared_ptr<const Ship> & ship, const Rect &screen_rect, s_ptr<std::unordered_map<ShipObject *, Vec2i>>& elements_positions) const;
    void GenerateCannonsPositions(const std::shared_ptr<const Ship> & ship, const Rect &screen_rect, s_ptr<std::unordered_map<ShipObject *, Vec2i>>& elements_positions) const;
    void DrawShipDeckFloor(const Rect &screen_rect) const;
    void DrawShipDeckCannons(const std::shared_ptr<const Ship> & ship, Rect screen_rect, const s_ptr<std::unordered_map<ShipObject *, Vec2i>>& elements_positions);
    void DrawShipDeckMasts(const std::shared_ptr<const Ship> & ship, Rect screen_rect, const s_ptr<std::unordered_map<ShipObject *, Vec2i>>& elements_positions);
    void DrawShipDeckSailors(const std::shared_ptr<const Ship> & ship, Rect screen_rect, const s_ptr<std::unordered_map<ShipObject *, Vec2i>>& elements_positions);

    void DrawCircleIndicator(Vec2i screen_coords, float arrow_angle, int diameter, const std::string &label,
                             const std::string &sub_label = "");
    void DrawWindDirIndicator(Vec2i offset, int size);
    void DrawShipDirIndicator(Vec2i offset, int size, const std::shared_ptr<const Ship> & ship);
    void DrawSailTargetDirIndicator(Vec2i offset, int size, const std::shared_ptr<const Ship> & ship);

    static Vec2i ToInverseYGlobal(Vec2i coords, int height);
    static Vec2i ToInverseY(Vec2i coords);
    static Vec2i GetWindowSize();

public:
    explicit Monitor(World * world);

    void NextShip();
    void PrevShip();
    void ChangeDisplayMode();
    MonitorDisplayMode GetDisplayMode();

    void MoveCameraUp();
    void MoveCameraDown();
    void MoveCameraLeft();
    void MoveCameraRight();
};


#endif //PIRATESSIMULATION_MONITOR_H
