//
// Created by konrad on 14.05.2021.
//

#include <ncurses.h>
#include <unistd.h>
#include "Monitor.h"
#include <thread>
#include <cmath>
#include <utility>
#include "Util/Vec2.h"
#include "Ship/Ship.h"
#include "Ship/Sailor.h"
#include "Ship/MastDistributor.h"
#include "Ship/Mast.h"
#include "Util/Util.h"
#include "Ship/Cannonball.h"
#include "Util/NcursesUtil.h"
#include "Util/Vec2i.h"
#include <sys/ioctl.h>

using namespace std;

const vector<int> Monitor::kSailorsColors = {COLOR_RED, COLOR_CYAN, COLOR_MAGENTA, COLOR_YELLOW, COLOR_GREEN};
std::unordered_map<SailorState, std::string> Monitor::sailor_states_map_strings = {
        {SailorState::kResting, "Resting"},
        {SailorState::kMast, "Operating mast"},
        {SailorState::kWalking, "Walking"},
        {SailorState::kCannon, "Operating cannon"},
        {SailorState::kWaitingCannon, "Waiting for cannon"},
        {SailorState::kStanding, "Standing"},
        {SailorState::kWaitingMast, "Waiting for mast"},
        {SailorState::kDead, "Dead"}
};

Monitor::Monitor(shared_ptr<World> world) {
    this->world = std::move(world);
}

void Monitor::Start() {
    InitializeNcurses();
    while(true)
    {
        if (stop) {
            endwin();
            return;
        }

        Update();
        SleepSeconds(0.08f);
    }
}

void Monitor::InitializeNcurses() {
    ncurses_util::Initialize();

    // Override colors' values
    init_color(COLOR_WHITE, 1000, 1000, 1000);
    init_color(COLOR_BLACK, 0, 0, 0);
    init_color(COLOR_RED, 1000, 350, 350);
    init_color(COLOR_CYAN, 250, 1000, 1000);
    init_color(COLOR_YELLOW, 1000, 1000, 250);
    init_color(COLOR_GREEN, 250, 1000, 250);
    init_color(COLOR_MAGENTA, 1000, 500, 1000);
    init_color(COLOR_BROWN, 500, 275, 0);
    init_color(COLOR_GRAY, 300, 300, 300);

    ncurses_util::InitAllPossibleColorPairs();

    // Override color pairs for convenient use with Tile enum
    init_pair(static_cast<short>(Tile::kWater), COLOR_CYAN, COLOR_BLUE );
    init_pair(static_cast<short>(Tile::kShip), COLOR_YELLOW, COLOR_BROWN );
    init_pair(static_cast<short>(Tile::kLand), COLOR_YELLOW, COLOR_GREEN);
    init_pair(static_cast<short>(Tile::kSail), COLOR_WHITE, COLOR_BROWN);
    init_pair(static_cast<short>(Tile::kGray), COLOR_GRAY, COLOR_GRAY);
    init_pair(static_cast<short>(Tile::kSailor), COLOR_YELLOW, COLOR_BLACK);
    init_pair(static_cast<short>(Tile::kStairs), COLOR_YELLOW, COLOR_BLACK);
    init_pair(static_cast<short>(Tile::kCannon), COLOR_WHITE, COLOR_BLACK);
    init_pair(static_cast<short>(Tile::kCannonball), COLOR_WHITE, COLOR_BLACK);
    init_pair(static_cast<short>(Tile::kDestroyed), COLOR_GRAY, COLOR_BLACK);
    init_pair(static_cast<short>(Tile::kIndicator), COLOR_WHITE, COLOR_WHITE);
}

void Monitor::Stop() {
    stop = true;
}

void Monitor::Update() {
    clear();

    if(display_mode == MonitorDisplayMode::kMap)
        DrawWorld();
    else
        DrawChosenShip();

    refresh();
}

void Monitor::DrawChosenShip() {
    shared_ptr<Ship> chosen_ship = nullptr;
    {
        lock_guard<mutex> ships_guard(world->ships_mutex);
        if(!world->ships.empty() && current_ship_ind < world->ships.size()){
            chosen_ship = world->ships.at(current_ship_ind);
        }
    }
    if(chosen_ship != nullptr)
        DrawShipInfo(chosen_ship);
}

void
Monitor::DrawMap(Vec2i screen_offset, Rect world_viewport) {
    for(int x = 0; x < world_viewport.width; x++){
        for(int y = 0; y < world_viewport.height; y++){
            Vec2i map_coords(world_viewport.x + x, world_viewport.y + y);
            Vec2i screen_coords(screen_offset.x + x, screen_offset.y + y);

            if(world->CorrectCoords(map_coords)){
                if(world->LandAt(map_coords))
                    DrawTile(screen_coords, ',', Tile::kLand);
                else
                    DrawTile(screen_coords, ';', Tile::kWater);
            } else
                DrawTile(screen_coords, ';', Tile::kGray);
        }
    }
}

void Monitor::DrawTile(Vec2i screen_coords, char character, Tile tile) {
    ncurses_util::AddTextColorPair(screen_coords, character, (int)tile);
}

void Monitor::DrawWorld(Vec2i screen_offset, Rect world_viewport) {
    // By deafult make drawn world match console size
    if(world_viewport.width == 0) {
        winsize win_size{};
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &win_size);
        world_viewport.width = win_size.ws_col;
    }
    if(world_viewport.height == 0) {
        winsize win_size{};
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &win_size);
        world_viewport.height = win_size.ws_row;
    }

    DrawMap(screen_offset, world_viewport);
    DrawShips(screen_offset, world_viewport);
    DrawCannonballs(screen_offset, world_viewport);
}

void Monitor::DrawShip(const shared_ptr<Ship>& ship, Vec2i screen_offset, Rect world_viewport) {
    Vec2 ship_pos = ship->GetPosition();
    Vec2 ship_dir = ship->GetDirection();

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
                Vec2 tile_rotated_local_pos = Vec2(
                        (float)j - length / 2,
                        (float)i - width / 2).Rotated(ship_dir.Angle());
                Vec2 tile_world_pos = Vec2(ship_pos.x + tile_rotated_local_pos.x, ship_pos.y + tile_rotated_local_pos.y);
                if(world_viewport.IsPointInside(tile_world_pos)){
                    Vec2i screen_coords = screen_offset + (Vec2i)(tile_world_pos - Vec2((float)world_viewport.x, (float)world_viewport.y));
                    Tile ship_tile = ship->GetState() != ShipState::kSinking && ship->GetState() != ShipState::kDestroyed ? Tile::kShip : Tile::kDestroyed;
                    DrawTile(screen_coords,pixel,ship_tile);
                }
            }
        }
    }
}

void Monitor::DrawShips(Vec2i offset, Rect world_viewport) {
    lock_guard<mutex> guard(world->ships_mutex);
    for(const shared_ptr<Ship>& ship : world->ships){
        DrawShip(ship, offset, world_viewport);
    }
}

void Monitor::DrawDashboard(shared_ptr <Ship> ship) {
    ncurses_util::ConsoleWriter console_writer({0, 0});
    string health = ship->GetState() == ShipState::kDestroyed ? "Zniszczony" : "Zdrowie: " + to_string(ship->GetHP());
    string header = "Statek(" + health + ")";
    console_writer.AddLine(header);
    console_writer.ModifyIndent(2);
    DrawDashboardSailors(ship, console_writer);
    DrawDashboardMasts(ship, console_writer);
    DrawDashboardCannons(ship, console_writer);
}

void Monitor::DrawDashboardSailors(shared_ptr<Ship> &ship, ncurses_util::ConsoleWriter &console_writer) {
    console_writer.AddLine("Marynarze:");
    console_writer.ModifyIndent(2);
    int sailor_index = 0;
    for(const auto& sailor : ship->GetSailors()){
        string state_text = sailor_states_map_strings.at(sailor->GetState());
        string sailor_text = "Marynarz " + to_string(sailor_index) + " " + state_text;
        auto sailor_color = GetSailorColor(sailor_index);
        console_writer.AddLine(sailor_text, sailor_color);
        sailor_index++;
    }
    console_writer.ModifyIndent(-2);
}

void Monitor::DrawDashboardMasts(shared_ptr<Ship> &ship, ncurses_util::ConsoleWriter &console_writer) const {
    console_writer.AddLine("Maszty:");
    console_writer.ModifyIndent(2);
    int mast_index = 0;
    {
        lock_guard<mutex> guard(ship->GetMastDistributor()->free_masts_mutex);
        for(const auto& mast_owners_pair : *ship->GetMastDistributor()->masts_owners){
            bool max_owners = mast_owners_pair.second->size() == mast_owners_pair.first->GetMaxSlots();
            string mast_text =
                    "Maszt " + to_string(mast_index) +
                    " Obslugiwany przez: " + to_string(mast_owners_pair.second->size()) +
                    " " + (max_owners ? "(max)" : "");
            console_writer.AddLine(mast_text);
            mast_index++;
        }
    }
    console_writer.ModifyIndent(-2);
}

void Monitor::DrawDashboardCannons(const vector<shared_ptr<Cannon>>& cannons, ncurses_util::ConsoleWriter &console_writer) const {
    int cannon_ind = 0;
    for(const auto& cannon : cannons){
        int owners = 0;
        auto cannon_owners = cannon->GetOwners();
        if(cannon_owners.first != nullptr)
            owners++;
        if(cannon_owners.second != nullptr)
            owners++;
        string cannon_message = "Armata " + to_string(cannon_ind) + " Obslugiwana przez: " + to_string(owners) +
                (owners == 2 ? "(Max)" : "");
        console_writer.AddLine(cannon_message);
        cannon_ind++;
    }
}

void Monitor::DrawShipDeck(shared_ptr <Ship> ship, Rect screen_rect) {
    s_ptr<std::unordered_map<shared_ptr<ShipObject>, Vec2i>> elements_positions = GenerateElementsPositions(ship, screen_rect);
    DrawShipDeckFloor(screen_rect);
    DrawShipDeckCannons(ship, screen_rect, elements_positions);
    DrawShipDeckMasts(ship, screen_rect, elements_positions);
    DrawShipDeckSailors(ship, screen_rect, elements_positions);
}

s_ptr<std::unordered_map<s_ptr<ShipObject>, Vec2i>> Monitor::GenerateElementsPositions(shared_ptr<Ship> &ship, const Rect &screen_rect) const {
    s_ptr<std::unordered_map<shared_ptr<ShipObject>, Vec2i>> elements_positions = make_shared<unordered_map<shared_ptr<ShipObject>, Vec2i>>();
    auto masts = ship->GetMasts();
    int first_mast_y, last_mast_y;
    if (masts.size() == 1)
        first_mast_y = screen_rect.height / 2;
    else {
        first_mast_y = (int)((float)screen_rect.height / 4.f);
        last_mast_y = (int)((float)screen_rect.height * 0.9f);
    }

    int mast_y = first_mast_y;
    for (const auto& mast: masts) {
        elements_positions->insert(make_pair(mast, Vec2((float)screen_rect.width / 2, (float)mast_y)));
        if (masts.size() > 1) {
            mast_y += (last_mast_y - first_mast_y) / (int)(masts.size() - 1);
        }
    }

    auto GenerateCannonsPositions = [&screen_rect, elements_positions] (const vector<shared_ptr<Cannon>>& cannons) {
        float cannon_y = (float)screen_rect.height / 3.f;
        float cannon_y_delta = (float)screen_rect.height / (float)(cannons.size() + 1);
        for (const auto& cannon: cannons) {
            elements_positions->insert(make_pair(cannon, Vec2((float)screen_rect.width - 1, (float)cannon_y)));
            cannon_y += cannon_y_delta;
        }
    };

    GenerateCannonsPositions(ship->GetLeftCannons());
    GenerateCannonsPositions(ship->GetRightCannons());

    elements_positions->insert(make_pair(ship->GetRightJunction(), Vec2((float)screen_rect.width * 0.75f, (float)screen_rect.height / 2.f)));
    elements_positions->insert(make_pair(ship->GetLeftJunction(), Vec2((float)screen_rect.width  * 0.25f, (float)screen_rect.height / 2.f)));
    elements_positions->insert(make_pair(ship->GetRestingPoint(), Vec2((float)screen_rect.width  * 0.75f, (float)screen_rect.height * 0.25f)));

    return elements_positions;
}

void Monitor::DrawShipDeckFloor(const Rect &screen_rect) const {
    char deck_ch = ' ';
    // Top part
    for(int y = 0; y < screen_rect.width /2; y++){
        for(int x = screen_rect.width /2 - y; x <= screen_rect.width /2 + y; x++){
            Vec2i screen_coords(x + screen_rect.x , y + screen_rect.y );
            DrawTile(screen_coords, deck_ch, Tile::kShip);
        }
    }

    // Middle part
    for(int y = screen_rect.width/2; y < screen_rect.height - screen_rect.width/4; y++){
        for(int x = 0; x < screen_rect.width; x++){
            Vec2i screen_coords(x + screen_rect.x, y + screen_rect.y);
            DrawTile(screen_coords, deck_ch, Tile::kShip);
        }
    }

    // Bottom part
    for(int y = screen_rect.height - screen_rect.width/4; y < screen_rect.height; y++){
        for(int x = screen_rect.width/4 - (screen_rect.height - y); x < screen_rect.width - screen_rect.width/4 + (screen_rect.height - y); x++){
            Vec2i screen_coords(x + screen_rect.x, y + screen_rect.y);
            DrawTile(screen_coords, deck_ch, Tile::kShip);
        }
    }
}

void Monitor::DrawCircleIndicator(Vec2i screen_coords, float arrow_angle, const string& label, int diameter) {
    int arrow_length = (diameter - 1) / 2;
    int angles = arrow_length * 7;
    Vec2i offset_for_centered_label((int)((float)diameter / 2.f - (float)label.length() / 2.f), 0);
    ncurses_util::AddText(screen_coords + offset_for_centered_label, label);
    Vec2i circle_center(screen_coords.x + arrow_length, screen_coords.y + arrow_length + 1);
    for(int i = 0; i < angles; i++){
        float angle = (float)i / (float)angles * 2.f * (float)M_PI;
        Vec2 pixel_local_pos = Vec2((float)arrow_length - 1.f, 0.f).Rotated(angle);
        Vec2i pixel_local_pos_rounded = Vec2i((int)roundf(pixel_local_pos.x), (int)roundf(pixel_local_pos.y));
        Vec2i pixel_screen_coords = circle_center + pixel_local_pos_rounded;
        DrawTile(pixel_screen_coords, '#', Tile::kGray);
    }
    for(int i = 0; i < arrow_length; i++){
        Vec2 pixel_local_pos = Vec2((float)i, 0).Rotated(arrow_angle);
        Vec2i pixel_local_pos_rounded = Vec2i((int)roundf(pixel_local_pos.x), (int)roundf(pixel_local_pos.y));
        Vec2i pixel_screen_coords = circle_center + pixel_local_pos_rounded;
        DrawTile(pixel_screen_coords, ' ', Tile::kIndicator);
    }
}

void Monitor::DrawWindDirIndicator(Vec2i offset, int size) {
    Vec2 wind_velocity = world->wind->GetVelocity();
    float wind_angle = wind_velocity.Angle();
    DrawCircleIndicator(offset, wind_angle, "Kierunek wiatru", size);
}

void Monitor::DrawShipDirIndicator(Vec2i offset, int size, const shared_ptr<Ship>& ship) {
    DrawCircleIndicator(offset, ship->GetDirection().Angle(), "Kierunek statku", size);
}

void Monitor::DrawSailTargetDirIndicator(Vec2i offset, int size, const std::shared_ptr<Ship>& ship) {
    float wind_angle = world->wind->GetVelocity().Angle();
    float ship_angle = ship->GetDirection().Angle();
    DrawCircleIndicator(offset, AngleDifference(wind_angle, ship_angle) - (float)M_PI / 2, "Docelowy kat zagli",
                        size);
}

void Monitor::DrawShipInfo(const shared_ptr<Ship>& ship) {
    DrawDashboard(ship);
    DrawShipDeck(ship, {40, 0, 29, 50});
    DrawWindDirIndicator({71, 0}, 18);
    DrawShipDirIndicator({71, 20}, 18, ship);
    DrawSailTargetDirIndicator({71, 40}, 18, ship);
    int map_view_size = 50;
    Rect viewport_centered_on_ship((int)ship->GetPosition().x - map_view_size / 2,
                                   (int)ship->GetPosition().y - map_view_size / 2,
                                   map_view_size, map_view_size);
    DrawWorld({90, 0},viewport_centered_on_ship);
}

void Monitor::DrawCannonballs(Vec2i screen_offset, Rect world_viewport) {
    lock_guard<mutex> guard(world->cannonballs_mutex);
    for(const auto& cannonball : world->cannonballs) {
        if(cannonball->dead)
            continue;
        Vec2 cannonball_pos = cannonball->GetPos();
        if(world_viewport.IsPointInside(cannonball_pos)) {
            Vec2i screen_coords = screen_offset - Vec2i(world_viewport.x, world_viewport.y) + (Vec2i)cannonball_pos;
            DrawTile(screen_coords, 'O', Tile::kCannonball);
        }
    }
}

void Monitor::NextShip() {
    current_ship_ind++;
    if(current_ship_ind >= world->ships.size())
        current_ship_ind = 0;
}

void Monitor::PrevShip() {
    current_ship_ind--;
    if(current_ship_ind < 0)
        current_ship_ind = max(0, (int)world->ships.size() - 1);
}

void Monitor::ChangeDisplayMode() {
    if(display_mode == MonitorDisplayMode::kMap)
        display_mode = MonitorDisplayMode::kDashboard;
    else
        display_mode = MonitorDisplayMode::kMap;
}

void Monitor::DrawDashboardCannons(const std::shared_ptr<Ship>& ship, ncurses_util::ConsoleWriter &console_writer) const {
    console_writer.AddLine("Armaty:");
    console_writer.ModifyIndent(1);

    console_writer.AddLine("Lewa strona:");
    console_writer.ModifyIndent(1);
    DrawDashboardCannons(ship->GetLeftCannons(), console_writer);
    console_writer.ModifyIndent(-1);

    console_writer.AddLine("Prawa strona:");
    console_writer.ModifyIndent(1);
    DrawDashboardCannons(ship->GetRightCannons(), console_writer);
    console_writer.ModifyIndent(-2);
}

void Monitor::DrawShipDeckCannons(const s_ptr<Ship>& ship, Rect screen_rect, const s_ptr<std::unordered_map<s_ptr<ShipObject>, Vec2i>>& elements_positions) {
    for(const auto& cannon : ship->GetRightCannons()){
        Vec2i cannon_pos = elements_positions->find(cannon)->second;
        Vec2i screen_coords(cannon_pos.x + screen_rect.x, cannon_pos.y + screen_rect.y);
        DrawTile(screen_coords, 'C', Tile::kCannon);
    }

    for(const auto& cannon : ship->GetLeftCannons()){
        Vec2i cannon_pos = elements_positions->find(cannon)->second;
        Vec2i screen_coords(cannon_pos.x + screen_rect.x, cannon_pos.y + screen_rect.y);
        DrawTile(screen_coords, 'C', Tile::kCannon);
    }
}

void Monitor::DrawShipDeckMasts(const s_ptr<Ship>& ship, Rect screen_rect,
                                const s_ptr<std::unordered_map<s_ptr<ShipObject>, Vec2i>>& elements_positions) {
    auto masts = ship->GetMasts();
    int mast_ind = 0;
    int mast_width = (int)((float)screen_rect.width * 0.9f) - 1;
    for(const auto& mast : masts){
        for(int i = 0; i < mast_width; i++){
            Vec2i rotated_sail_pos = (Vec2i)Vec2((float)i - (float)mast_width / 2.f, 0.f).Rotated(mast->GetAngle());
            char ch = i < mast_width / 2 ? 'L' : 'R';
            Vec2i mast_pos = elements_positions->find(mast)->second;
            Vec2i screen_coords = Vec2i(screen_rect.x, screen_rect.y) + mast_pos + rotated_sail_pos;
            DrawTile(screen_coords, ch, Tile::kSail);
        }
    }
}

void Monitor::DrawShipDeckSailors(const s_ptr<Ship>& ship, Rect screen_rect,
                                  const s_ptr<std::unordered_map<s_ptr<ShipObject>, Vec2i>>& elements_positions) {
    int sailor_index = 0;
    for(const auto& sailor : ship->GetSailors()){
        Vec2i current_sailor_pos;
        SailorState sailor_state = sailor->GetState();
        if(sailor_state == SailorState::kCannon){
            Vec2i cannon_pos = elements_positions->find(sailor->GetOperatedCannon())->second;
            if(sailor->GetOperatedCannon()->GetOwners().first == sailor.get()){
                current_sailor_pos = cannon_pos + Vec2i(0, 1);
            }else
                current_sailor_pos = cannon_pos - Vec2i(0, 1);
        }else if(sailor_state == SailorState::kMast){
            auto mast_owners = ship->GetMastDistributor()->masts_owners->at(sailor->GetOperatedMast());
            Vec2i mast_pos = elements_positions->at(sailor->GetOperatedMast());

            int ind = 0;
            for(auto mast_owner : *mast_owners){
                if(mast_owner == sailor.get())
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
            Vec2i prev_target_pos = elements_positions->find(sailor->GetPreviousTarget())->second;
            Vec2i next_target_pos = elements_positions->find(sailor->GetNextTarget())->second;
            float progress = sailor->GetProgress();

            current_sailor_pos = prev_target_pos + (Vec2i)((next_target_pos - prev_target_pos) * progress);
        }
        Vec2i sailor_screen_coords = (Vec2i)current_sailor_pos + Vec2i(screen_rect.x, screen_rect.y);
        ncurses_util::AddText(sailor_screen_coords, "S", GetSailorColor(sailor_index));
        ++sailor_index;
    }
}

int Monitor::GetSailorColor(int sailor_index) {
    return kSailorsColors.at(sailor_index % kSailorsColors.size());
}

