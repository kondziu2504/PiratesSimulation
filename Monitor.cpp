//
// Created by konrad on 14.05.2021.
//

#include <ncurses.h>
#include <unistd.h>
#include "Monitor.h"
#include <thread>
#include <cmath>
#include <utility>
#include "Util/Vec2f.h"
#include "Ship/Ship.h"
#include "Ship/Sailor.h"
#include "Ship/MastDistributor.h"
#include "Ship/Mast.h"
#include "Util/Util.h"
#include "Ship/Cannonball.h"
#include "Util/NcursesUtil.h"
#include "Util/Vec2i.h"
#include <sys/ioctl.h>


#define COLOR_BROWN 8
#define COLOR_GRAY 9

using namespace std;

const vector<int> Monitor::kSailorsColors = {COLOR_RED, COLOR_CYAN, COLOR_MAGENTA, COLOR_YELLOW, COLOR_GREEN};

const std::unordered_map<Monitor::Tile, int> Monitor::tiles_map_color_pairs = {
        {Monitor::Tile::kWater, ncurses_util::ColorNum(COLOR_CYAN, COLOR_BLUE)},
        {Monitor::Tile::kShip, ncurses_util::ColorNum(COLOR_YELLOW, COLOR_BROWN)},
        {Monitor::Tile::kLand, ncurses_util::ColorNum(COLOR_YELLOW, COLOR_GREEN)},
        {Monitor::Tile::kSail, ncurses_util::ColorNum( COLOR_WHITE, COLOR_BROWN)},
        {Monitor::Tile::kGray, ncurses_util::ColorNum(COLOR_GRAY, COLOR_GRAY)},
        {Monitor::Tile::kSailor, ncurses_util::ColorNum(COLOR_YELLOW, COLOR_BLACK)},
        {Monitor::Tile::kCannon, ncurses_util::ColorNum(COLOR_WHITE, COLOR_BLACK)},
        {Monitor::Tile::kCannonball, ncurses_util::ColorNum(COLOR_WHITE, COLOR_BLACK)},
        {Monitor::Tile::kDestroyed, ncurses_util::ColorNum(COLOR_GRAY, COLOR_BLACK)},
        {Monitor::Tile::kIndicator, ncurses_util::ColorNum(COLOR_WHITE, COLOR_WHITE)}
};

const std::unordered_map<SailorState, std::string> Monitor::sailor_states_map_strings = {
        {SailorState::kResting, "Resting"},
        {SailorState::kMast, "Operating mast"},
        {SailorState::kWalking, "Walking"},
        {SailorState::kCannon, "Operating cannon"},
        {SailorState::kWaitingCannon, "Waiting for cannon"},
        {SailorState::kStanding, "Standing"},
        {SailorState::kWaitingMast, "Waiting for mast"},
        {SailorState::kDead, "Dead"}
};

Monitor::Monitor(World * world) : world(world) { }

void Monitor::InitializeNcurses() {
    ncurses_util::InitAllPossibleColorPairs();

    // Override default colors' values
    init_color(COLOR_WHITE, 1000, 1000, 1000);
    init_color(COLOR_BLACK, 0, 0, 0);
    init_color(COLOR_RED, 1000, 350, 350);
    init_color(COLOR_CYAN, 250, 1000, 1000);
    init_color(COLOR_YELLOW, 1000, 1000, 250);
    init_color(COLOR_GREEN, 250, 1000, 250);
    init_color(COLOR_MAGENTA, 1000, 500, 1000);
    // Init custom colors
    init_color(COLOR_BROWN, 500, 275, 0);
    init_color(COLOR_GRAY, 300, 300, 300);
}

void Monitor::Update() {
    clear();

    if(display_mode == MonitorDisplayMode::kMap)
        DrawWorld({0, 0}, {camera_pos - Vec2i(0, GetWindowSize().y - world->GetHeight()) , {0, 0}});
    else
        DrawChosenShip();

    refresh();
}

void Monitor::DrawChosenShip() {
    shared_ptr<Ship> _current_ship;
    {
        lock_guard<mutex> guard(current_ship_mutex);
        _current_ship = current_ship.lock();
        if (!_current_ship) {
            auto ships = world->GetShips();
            if (!ships.empty()) {
                for (auto ship: ships) {
                    auto _ship = ship.lock();
                    if (_ship) {
                        current_ship = ship;
                        _current_ship = _ship;
                        break;
                    }
                }
            }
        }
    }
    if(_current_ship)
        DrawShipInfo(_current_ship);
}

void Monitor::DrawMap(Vec2i screen_offset, Rect world_viewport) {
    for(int x = 0; x < world_viewport.size.x; x++){
        for(int y = 0; y < world_viewport.size.y; y++){
            Vec2i map_coords = world_viewport.pos + Vec2i(x, y);
            Vec2i screen_coords = ToInverseY(screen_offset + Vec2i(x, y), world_viewport.size.y);

            if(world->CoordsInsideWorld(map_coords)){
                if(world->IsLandAt(map_coords))
                    DrawTile(screen_coords, ',', Tile::kLand);
                else
                    DrawTile(screen_coords, ';', Tile::kWater);
            } else
                DrawTile(screen_coords, ';', Tile::kGray);
        }
    }
}

void Monitor::DrawTile(Vec2i screen_coords, char character, Tile tile) {
    ncurses_util::AddTextColorPair(screen_coords, character, GetTileColorPair(tile));
}

void Monitor::DrawWorld(Vec2i screen_offset, Rect world_viewport) {
    // By default, make drawn world match console size
    if(world_viewport.size.x == 0) {
        world_viewport.size.x = GetWindowSize().x;
    }
    if(world_viewport.size.y == 0) {
        world_viewport.size.y = GetWindowSize().y;
    }

    DrawMap(screen_offset, world_viewport);
    DrawShips(screen_offset, world_viewport);
    DrawCannonballs(screen_offset, world_viewport);
}

void Monitor::DrawShip(const std::shared_ptr<const Ship> & ship, Vec2i screen_offset, Rect world_viewport) {
    Vec2f ship_pos = ship->GetPosition();
    Vec2f ship_dir = ship->GetDirection();

    vector<string> texture = {
            "TTMMMFF",
            "TTMMMFF"
    };

    for(int i = 0; i < texture.size(); i++){
        for(int j = 0; j < texture[i].length(); j++){
            char pixel = texture[i][j];
            if(pixel != ' '){
                auto length = (float)texture[i].length();
                auto width = (float)texture.size();
                Vec2f tile_rotated_local_pos = Vec2f(
                        (float)j - length / 2,
                        (float)i - width / 2).Rotated(ship_dir.Angle());
                Vec2f tile_world_pos = Vec2f(ship_pos.x + tile_rotated_local_pos.x, ship_pos.y + tile_rotated_local_pos.y);
                if(world_viewport.IsPointInside(tile_world_pos)){
                    Vec2i screen_coords = ToInverseY(screen_offset + (Vec2i)(tile_world_pos - (Vec2f)world_viewport.pos), world_viewport.size.y);
                    Tile ship_tile = (ship->GetState() != ShipState::kSinking && ship->GetState() != ShipState::kDestroyed) ? Tile::kShip : Tile::kDestroyed;
                    DrawTile(screen_coords,pixel,ship_tile);
                }
            }
        }
    }
}

void Monitor::DrawShips(Vec2i screen_offset, Rect world_viewport) {
    for(const auto ship : world->GetShips()) {
        auto _ship = ship.lock();
        if(_ship)
            DrawShip(_ship, screen_offset, world_viewport);
    }
}

void Monitor::DrawDashboard(const std::shared_ptr<const Ship> & ship) {
    ncurses_util::ConsoleWriter console_writer({0, 0});
    string health = ship->GetState() == ShipState::kDestroyed ? "Destroyed" : "Health: " + to_string(ship->GetHP());
    string header = "Ship(" + health + ")";
    console_writer.AddLine(header);
    console_writer.ModifyIndent(2);
    DrawDashboardSailors(ship, console_writer);
    DrawDashboardMasts(ship, console_writer);
    DrawDashboardCannons(ship, console_writer);
}

void Monitor::DrawDashboardSailors(const std::shared_ptr<const Ship> & ship, ncurses_util::ConsoleWriter &console_writer) {
    console_writer.AddLine("Sailors:");
    console_writer.ModifyIndent(2);
    int sailor_index = 0;
    for(const auto& sailor : ship->GetSailors()){
        string state_text = sailor_states_map_strings.at(sailor->GetState());
        string sailor_text = "Sailor " + to_string(sailor_index) + " " + state_text;
        auto sailor_color = GetSailorColor(sailor_index);
        console_writer.AddLine(sailor_text, sailor_color);
        sailor_index++;
    }
    console_writer.ModifyIndent(-2);
}

void Monitor::DrawDashboardMasts(const std::shared_ptr<const Ship> & ship, ncurses_util::ConsoleWriter &console_writer) const {
    console_writer.AddLine("Masts:");
    console_writer.ModifyIndent(2);

    int mast_index = 0;
    for(const auto& mast_owners_pair : ship->GetMastDistributor()->GetMastsOwners()) {
        bool max_owners = mast_owners_pair.second.size() == mast_owners_pair.first->GetMaxSlots();
        string mast_text =
                "Mast " + to_string(mast_index) +
                " Operated by: " + to_string(mast_owners_pair.second.size()) +
                " " + (max_owners ? "(max)" : "");
        console_writer.AddLine(mast_text);
        mast_index++;
    }

    console_writer.ModifyIndent(-2);
}

void Monitor::DrawDashboardCannons(const vector<Cannon *>& cannons, ncurses_util::ConsoleWriter &console_writer) const {
    int cannon_ind = 0;
    for(const auto& cannon : cannons){
        int owners = 0;
        auto cannon_owners = cannon->GetOwners();
        if(cannon_owners.first != nullptr)
            owners++;
        if(cannon_owners.second != nullptr)
            owners++;
        string cannon_message = "Cannon " + to_string(cannon_ind) + " Operated by: " + to_string(owners) +
                (owners == 2 ? "(Max)" : "");
        console_writer.AddLine(cannon_message);
        cannon_ind++;
    }
}

void Monitor::DrawShipDeck(const std::shared_ptr<const Ship> & ship, Rect screen_rect) {
    s_ptr<std::unordered_map<ShipObject *, Vec2i>> elements_positions = GenerateElementsPositions(ship, screen_rect);
    DrawShipDeckFloor(screen_rect);
    DrawShipDeckCannons(ship, screen_rect, elements_positions);
    DrawShipDeckMasts(ship, screen_rect, elements_positions);
    DrawShipDeckSailors(ship, screen_rect, elements_positions);
}

s_ptr<std::unordered_map<ShipObject *, Vec2i>> Monitor::GenerateElementsPositions(const std::shared_ptr<const Ship> & ship, const Rect &screen_rect) const {
    s_ptr<std::unordered_map<ShipObject *, Vec2i>> elements_positions = make_shared<unordered_map<ShipObject *, Vec2i>>();
    GenerateMastsPositions(ship, screen_rect, elements_positions);
    GenerateCannonsPositions(ship, screen_rect, elements_positions);

    elements_positions->insert(make_pair(ship->GetRightJunction(), Vec2f((float)screen_rect.size.x * 0.75f, (float)screen_rect.size.y / 2.f)));
    elements_positions->insert(make_pair(ship->GetLeftJunction(), Vec2f((float)screen_rect.size.x  * 0.25f, (float)screen_rect.size.y / 2.f)));
    elements_positions->insert(make_pair(ship->GetRestingPoint(), Vec2f((float)screen_rect.size.x  * 0.75f, (float)screen_rect.size.y * 0.25f)));

    return elements_positions;
}


void Monitor::GenerateMastsPositions(const shared_ptr<const Ship> &ship, const Rect &screen_rect,
                                     s_ptr<unordered_map<ShipObject *, Vec2i>> &elements_positions) const {
    auto masts = ship->GetMasts();
    int first_mast_y, last_mast_y;
    if (masts.size() == 1)
        first_mast_y = screen_rect.size.y / 2;
    else {
        first_mast_y = (int)((float)screen_rect.size.y / 4.f);
        last_mast_y = (int)((float)screen_rect.size.y * 0.9f);
    }

    int mast_y = first_mast_y;
    for (const auto& mast: masts) {
        elements_positions->insert(make_pair(mast, Vec2f((float)screen_rect.size.x / 2, (float)mast_y)));
        if (masts.size() > 1) {
            mast_y += (last_mast_y - first_mast_y) / (int)(masts.size() - 1);
        }
    }
}


void Monitor::GenerateCannonsPositions(const shared_ptr<const Ship> &ship, const Rect &screen_rect,
                                       s_ptr<unordered_map<ShipObject *, Vec2i>> &elements_positions) const {
    auto GenerateSideCannonsPositions = [&screen_rect, elements_positions] (const vector<Cannon *>& cannons, bool right) {
        float cannon_y_delta = ((float)screen_rect.size.y - 0.75f * (float)screen_rect.size.x) / (float)(cannons.size());
        float cannon_y = (float)screen_rect.size.x / 2.f + cannon_y_delta / 2.f;
        for (const auto& cannon: cannons) {
            if(right)
                elements_positions->insert(make_pair(cannon, Vec2f((float)screen_rect.size.x - 1, (float)cannon_y)));
            else
                elements_positions->insert(make_pair(cannon, Vec2f(0, (float)cannon_y)));

            cannon_y += cannon_y_delta;
        }
    };

    GenerateSideCannonsPositions(ship->GetLeftCannons(), false);
    GenerateSideCannonsPositions(ship->GetRightCannons(), true);
}

void Monitor::DrawShipDeckFloor(const Rect &screen_rect) const {
    char deck_ch = ' ';
    // Top part
    for(int y = 0; y < screen_rect.size.x /2; y++){
        for(int x = screen_rect.size.x /2 - y; x <= screen_rect.size.x /2 + y; x++){
            Vec2i screen_coords = screen_rect.pos + Vec2i(x, y);
            DrawTile(screen_coords, deck_ch, Tile::kShip);
        }
    }

    // Middle part
    for(int y = screen_rect.size.x/2; y < screen_rect.size.y - screen_rect.size.x/4; y++){
        for(int x = 0; x < screen_rect.size.x; x++){
            Vec2i screen_coords = screen_rect.pos + Vec2i(x, y);
            DrawTile(screen_coords, deck_ch, Tile::kShip);
        }
    }

    // Bottom part
    for(int y = screen_rect.size.y - screen_rect.size.x/4; y < screen_rect.size.y; y++){
        for(int x = screen_rect.size.x/4 - (screen_rect.size.y - y); x < screen_rect.size.x - screen_rect.size.x/4 + (screen_rect.size.y - y); x++){
            Vec2i screen_coords = screen_rect.pos + Vec2i(x, y);
            DrawTile(screen_coords, deck_ch, Tile::kShip);
        }
    }
}

void Monitor::DrawCircleIndicator(Vec2i screen_coords, float arrow_angle, int diameter, const std::string &label,
                                  const std::string &sub_label) {
    int arrow_length = (diameter - 1) / 2;
    int angles = arrow_length * 7;

    Vec2i offset_for_centered_label((int)((float)diameter / 2.f - (float)label.length() / 2.f), 0);
    ncurses_util::AddText(screen_coords + offset_for_centered_label, label);
    Vec2i offset_for_centered_sub_label((int)((float)diameter / 2.f - (float)sub_label.length() / 2.f), 1);
    ncurses_util::AddText(screen_coords + offset_for_centered_sub_label, sub_label);

    Vec2i circle_center(screen_coords.x + arrow_length, screen_coords.y + arrow_length + 1);
    for(int i = 0; i < angles; i++){
        float angle = (float)i / (float)angles * 2.f * (float)M_PI;
        Vec2f pixel_local_pos = Vec2f((float)arrow_length - 1.f, 0.f).Rotated(angle);
        Vec2i pixel_local_pos_rounded = Vec2i((int)roundf(pixel_local_pos.x), (int)roundf(pixel_local_pos.y));
        Vec2i pixel_screen_coords = circle_center + pixel_local_pos_rounded;
        DrawTile(pixel_screen_coords, '#', Tile::kGray);
    }
    for(int i = 0; i < arrow_length; i++){
        Vec2f pixel_local_pos = Vec2f((float)i, 0).Rotated(arrow_angle);
        Vec2i pixel_local_pos_rounded = Vec2i((int)roundf(pixel_local_pos.x), (int)roundf(pixel_local_pos.y));
        Vec2i pixel_screen_coords = circle_center + pixel_local_pos_rounded;
        DrawTile(pixel_screen_coords, ' ', Tile::kIndicator);
    }
}

void Monitor::DrawWindDirIndicator(Vec2i offset, int size) {
    Vec2f wind_velocity = world->GetWind()->GetVelocity();
    float wind_angle = wind_velocity.Angle();
    DrawCircleIndicator(offset, wind_angle, size, "Wind direction", "(Global)");
}

void Monitor::DrawShipDirIndicator(Vec2i offset, int size, const std::shared_ptr<const Ship> & ship) {
    DrawCircleIndicator(offset, ship->GetDirection().Angle(), size, "Ship direction", "(Global)");
}

void Monitor::DrawSailTargetDirIndicator(Vec2i offset, int size, const std::shared_ptr<const Ship> & ship) {
    float wind_angle = world->GetWind()->GetVelocity().Angle();
    float ship_angle = ship->GetDirection().Angle();
    DrawCircleIndicator(offset, AngleDifference(wind_angle, ship_angle) - (float) M_PI / 2,
                        size, "Target sails direction", "(Local)");
}

void Monitor::DrawShipInfo(const std::shared_ptr<const Ship> & ship) {
    DrawDashboard(ship);
    DrawShipDeck(ship, {36, 0, 29, 50});
    DrawWindDirIndicator({69, 0}, 18);
    DrawShipDirIndicator({69, 20}, 18, ship);
    DrawSailTargetDirIndicator({69, 40}, 18, ship);
    int map_view_size = 50;
    Rect viewport_centered_on_ship((int)ship->GetPosition().x - map_view_size / 2,
                                   (int)ship->GetPosition().y - map_view_size / 2,
                                   map_view_size, map_view_size);
    DrawWorld({90, 0},viewport_centered_on_ship);
}

void Monitor::DrawCannonballs(Vec2i screen_offset, Rect world_viewport) {
    for(const auto& cannonball : world->GetCannonballs()) {
        if(cannonball->dead)
            continue;
        Vec2f cannonball_pos = cannonball->GetPos();
        if(world_viewport.IsPointInside(cannonball_pos)) {
            Vec2i screen_coords = ToInverseY(screen_offset - world_viewport.pos + (Vec2i)cannonball_pos, world_viewport.size.y);
            DrawTile(screen_coords, 'O', Tile::kCannonball);
        }
    }
}

void Monitor::NextShip() {
    auto ships = world->GetShips();
    lock_guard<mutex> guard(current_ship_mutex);
    auto cur_ship_it = std::find_if(ships.begin(), ships.end(),
                                    [this](weak_ptr<Ship> & ship) {
                                        auto _ship = ship.lock();
                                        return (_ship == current_ship.lock()) && _ship != nullptr;
                                    });

    if(cur_ship_it == ships.end())
        return;

    if(++cur_ship_it != ships.end())
        current_ship = *cur_ship_it;
    else
        current_ship = *ships.begin();
}

void Monitor::PrevShip() {
    auto ships = world->GetShips();
    lock_guard<mutex> guard(current_ship_mutex);
    auto cur_ship_it = std::find_if(ships.begin(), ships.end(),
                                    [this](weak_ptr<Ship> & ship) {
                                        auto _ship = ship.lock();
                                        return (_ship == current_ship.lock()) && _ship != nullptr;
                                    });

    if(cur_ship_it == ships.end())
        return;

    if(cur_ship_it != ships.begin())
        current_ship = *(--cur_ship_it);
    else
        current_ship = *(--ships.end());
}

void Monitor::ChangeDisplayMode() {
    if(display_mode == MonitorDisplayMode::kMap)
        display_mode = MonitorDisplayMode::kDashboard;
    else
        display_mode = MonitorDisplayMode::kMap;
}

void Monitor::DrawDashboardCannons(const std::shared_ptr<const Ship> & ship, ncurses_util::ConsoleWriter &console_writer) const {
    console_writer.AddLine("Cannons:");
    console_writer.ModifyIndent(1);

    console_writer.AddLine("Left side:");
    console_writer.ModifyIndent(1);
    DrawDashboardCannons(ship->GetLeftCannons(), console_writer);
    console_writer.ModifyIndent(-1);

    console_writer.AddLine("Right side:");
    console_writer.ModifyIndent(1);
    DrawDashboardCannons(ship->GetRightCannons(), console_writer);
    console_writer.ModifyIndent(-2);
}

void Monitor::DrawShipDeckCannons(const std::shared_ptr<const Ship> & ship, Rect screen_rect, const s_ptr<std::unordered_map<ShipObject *, Vec2i>>& elements_positions) {
    for(const auto& cannon : ship->GetRightCannons()){
        Vec2i cannon_pos = elements_positions->find(cannon)->second;
        Vec2i screen_coords = screen_rect.pos + cannon_pos;
        DrawTile(screen_coords, 'C', Tile::kCannon);
    }

    for(const auto& cannon : ship->GetLeftCannons()){
        Vec2i cannon_pos = elements_positions->find(cannon)->second;
        Vec2i screen_coords = screen_rect.pos + cannon_pos;
        DrawTile(screen_coords, 'C', Tile::kCannon);
    }
}

void Monitor::DrawShipDeckMasts(const std::shared_ptr<const Ship> & ship, Rect screen_rect,
                                const s_ptr<std::unordered_map<ShipObject *, Vec2i>>& elements_positions) {
    auto masts = ship->GetMasts();
    int mast_ind = 0;
    int mast_width = (int)((float)screen_rect.size.x * 0.9f) - 1;
    for(const auto& mast : masts){
        for(int i = 0; i < mast_width; i++){
            Vec2i rotated_sail_pos = (Vec2i)Vec2f((float)i - (float)mast_width / 2.f, 0.f).Rotated(mast->GetAngle());
            char ch = i < mast_width / 2 ? 'L' : 'R';
            Vec2i mast_pos = elements_positions->find(mast)->second;
            Vec2i screen_coords = screen_rect.pos + mast_pos + rotated_sail_pos;
            DrawTile(screen_coords, ch, Tile::kSail);
        }
    }
}

void Monitor::DrawShipDeckSailors(const std::shared_ptr<const Ship> & ship, Rect screen_rect,
                                  const s_ptr<std::unordered_map<ShipObject *, Vec2i>>& elements_positions) {
    int sailor_index = 0;
    for(const auto& sailor : ship->GetSailors()){
        Vec2i current_sailor_pos;
        SailorState sailor_state = sailor->GetState();
        if(sailor_state == SailorState::kCannon){
            Vec2i cannon_pos = elements_positions->find(sailor->GetOperatedCannon())->second;
            if(sailor->GetOperatedCannon()->GetOwners().first == sailor){
                current_sailor_pos = cannon_pos + Vec2i(0, 1);
            }else
                current_sailor_pos = cannon_pos - Vec2i(0, 1);
        }else if(sailor_state == SailorState::kMast){
            auto mast_owners = ship->GetMastDistributor()->GetMastsOwners().at(sailor->GetOperatedMast());
            Vec2i mast_pos = elements_positions->at(sailor->GetOperatedMast());

            // Find out at which position sailor operates mast
            int ind = 0;
            for(auto mast_owner : mast_owners){
                if(mast_owner == sailor)
                    break;
                ind++;
            }

            switch(ind){
                case 0: current_sailor_pos = mast_pos + Vec2i(0, -1); break;
                case 1: current_sailor_pos = mast_pos + Vec2i(1, 0); break;
                case 2: current_sailor_pos = mast_pos + Vec2i(0, 1); break;
                case 3: current_sailor_pos = mast_pos + Vec2i(-1, 0); break;
                default: break;
            }
        } else{
            auto prev_target = sailor->GetPreviousTarget();
            Vec2i prev_target_pos = elements_positions->find(prev_target != nullptr ? prev_target : ship->GetRestingPoint())->second;
            auto next_target = sailor->GetNextTarget();
            Vec2i next_target_pos = elements_positions->find(next_target != nullptr ? next_target : ship->GetRestingPoint())->second;
            float progress = sailor->GetProgress();

            current_sailor_pos = prev_target_pos + (Vec2i)((next_target_pos - prev_target_pos) * progress);
        }
        Vec2i sailor_screen_coords = (Vec2i)current_sailor_pos + screen_rect.pos;
        ncurses_util::AddText(sailor_screen_coords, 'S', GetSailorColor(sailor_index));
        ++sailor_index;
    }
}

int Monitor::GetSailorColor(int sailor_index) {
    return kSailorsColors.at(sailor_index % kSailorsColors.size());
}

void Monitor::ThreadFunc(const atomic<bool> &stop_requested) {
    InitializeNcurses();
    while(true)
    {
        if (stop_requested) {
            endwin();
            return;
        }

        Update();
        SleepSeconds(0.08f);
    }
}

int Monitor::GetTileColorPair(Monitor::Tile tile) {
    return tiles_map_color_pairs.at(tile);
}

void Monitor::MoveCameraUp() {
    camera_pos.y += kCameraMoveDelta;
}

void Monitor::MoveCameraDown() {
    camera_pos.y -= kCameraMoveDelta;
}

void Monitor::MoveCameraLeft() {
    camera_pos.x -= kCameraMoveDelta;
}

void Monitor::MoveCameraRight() {
    camera_pos.x += kCameraMoveDelta;
}

Vec2i Monitor::ToInverseY(Vec2i coords, int height) {
    coords.y *= -1;
    coords.y += height - 1;
    return coords;
}

Vec2i Monitor::GetWindowSize() {
    winsize win_size{};
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &win_size);
    return {win_size.ws_col, win_size.ws_row};
}

MonitorDisplayMode Monitor::GetDisplayMode() {
    return display_mode;
}



