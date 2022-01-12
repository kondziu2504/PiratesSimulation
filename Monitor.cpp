//
// Created by konrad on 14.05.2021.
//

#include <ncurses.h>
#include <unistd.h>
#include "Monitor.h"
#include <thread>
#include <cmath>
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
unordered_map<Sailor *, int> Monitor::sailors_assigned_colors = unordered_map<Sailor*, int>();
int Monitor::color_id = 0;

Monitor::Monitor(shared_ptr<World> world) {
    this->world = world;
}

void Monitor::Start() {
    Initialize();
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

void Monitor::Initialize() {
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
    attron(COLOR_PAIR((int)tile));
    mvaddch(screen_coords.y, screen_coords.x, character);
    attroff(COLOR_PAIR((int)tile));
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

void Monitor::DrawShip(shared_ptr<Ship> ship, Vec2i screen_offset, Rect world_viewport) {
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
                float length = texture[i].length();
                float width = texture.size();
                Vec2 tile_rotated_local_pos = Vec2(
                        j - length / 2,
                        i - width / 2).Rotated(ship_dir.Angle());
                Vec2 tile_world_pos = Vec2((int)ship_pos.x + tile_rotated_local_pos.x, (int)ship_pos.y + tile_rotated_local_pos.y);
                if(world_viewport.IsPointInside(tile_world_pos)){
                    Vec2i screen_coords(tile_world_pos.x + screen_offset.x - world_viewport.x, tile_world_pos.y + screen_offset.y - world_viewport.y);
                    Tile ship_tile = ship->GetState() != ShipState::kSinking && ship->GetState() != ShipState::kDestroyed ? Tile::kShip : Tile::kDestroyed;
                    DrawTile(screen_coords,pixel,ship_tile);
                }
            }
        }
    }
}

void Monitor::DrawShips(Vec2i offset, Rect world_viewport) {
    lock_guard<mutex> guard(world->ships_mutex);
    for(shared_ptr<Ship> ship : world->ships){
        DrawShip(ship, offset, world_viewport);
    }
}



void Monitor::DrawDashboard(shared_ptr <Ship> ship) {
    ncurses_util::ConsoleWriter console_writer({0, 0});

    string health = ship->GetState() == ShipState::kDestroyed ? "Zniszczony" : "Zdrowie: " + to_string(ship->GetHP());
    string header = "Statek(" + health + ")";
    console_writer.AddLine(header);
    console_writer.ModifyIndent(2);
    console_writer.AddLine("Marynarze:");
    console_writer.ModifyIndent(2);

    int sailor_index = 0;
    for(const auto& sailor : ship->GetSailors()){
        string state_text = sailor_states_map_strings.at(sailor->GetState());
        string sailor_text = "Marynarz " + to_string(sailor_index) + " " + state_text;
        auto sailor_color = GetColor(sailor.get());
        console_writer.AddLine(sailor_text, sailor_color);
        sailor_index++;
    }

    console_writer.ModifyIndent(-2);
    console_writer.AddLine("Maszty:");
    int mast_index = 0;
    console_writer.ModifyIndent(2);
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
    console_writer.AddLine("Armaty:");
    console_writer.ModifyIndent(1);
    console_writer.AddLine("Lewa strona:");
    console_writer.ModifyIndent(1);
    int cannon_ind = 0;
    for(const auto& cannon : ship->GetLeftCannons()){
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
    console_writer.ModifyIndent(-1);
    console_writer.AddLine("Prawa strona:");
    console_writer.ModifyIndent(1);
    cannon_ind = 0;
    for(const auto& cannon : ship->GetRightCannons()){
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
    s_ptr<std::unordered_map<shared_ptr<ShipObject>, Vec2>> elements_positions = make_shared<unordered_map<shared_ptr<ShipObject>, Vec2>>();

    {
        //Generate positions
        auto masts = ship->GetMasts();
        int first_mast_y, last_mast_y;
        if (masts.size() == 1)
            first_mast_y = screen_rect.height / 2;
        else {
            first_mast_y = screen_rect.height / 4;
            last_mast_y = screen_rect.height * 0.9f;
        }

        int mast_y = first_mast_y;
        for (auto mast: masts) {
            elements_positions->insert(make_pair(mast, Vec2(screen_rect.width / 2, mast_y)));
            if (masts.size() > 1) {
                mast_y += (last_mast_y - first_mast_y) / (masts.size() - 1);
            }
        }

        int cannon_y = screen_rect.height / 3;
        auto right_cannons = ship->GetRightCannons();
        int cannon_y_delta = screen_rect.height / (right_cannons.size() + 1);
        for (auto cannon: right_cannons) {
            elements_positions->insert(make_pair(cannon, Vec2(screen_rect.width - 1, cannon_y)));
            cannon_y += cannon_y_delta;
        }

        cannon_y = screen_rect.height / 3;
        auto left_cannons = ship->GetLeftCannons();
        cannon_y_delta = screen_rect.height / (left_cannons.size() + 1);
        for (auto cannon: left_cannons) {
            elements_positions->insert(make_pair(cannon, Vec2(0, cannon_y)));
            cannon_y += cannon_y_delta;
        }

        elements_positions->insert(make_pair(ship->GetRightJunction(), Vec2(screen_rect.width * 0.75, screen_rect.height / 2)));
        elements_positions->insert(make_pair(ship->GetLeftJunction(), Vec2(screen_rect.width  * 0.25, screen_rect.height / 2)));
        elements_positions->insert(make_pair(ship->GetRestingPoint(), Vec2(screen_rect.width  * 0.75, screen_rect.height * 0.25)));
    }

    char deck_ch = ' ';
    for(int y = 0; y < screen_rect.width /2; y++){
        for(int x = screen_rect.width /2 - y; x <= screen_rect.width /2 + y; x++){
            Vec2i screen_coords(x + screen_rect.x , y + screen_rect.y );
            DrawTile(screen_coords, deck_ch, Tile::kShip);
        }
    }

    for(int y = screen_rect.width/2; y < screen_rect.height - screen_rect.width/4; y++){
        for(int x = 0; x < screen_rect.width; x++){
            Vec2i screen_coords(x + screen_rect.x, y + screen_rect.y);
            DrawTile(screen_coords, deck_ch, Tile::kShip);
        }
    }

    for(int y = screen_rect.height - screen_rect.width/4; y < screen_rect.height; y++){
        for(int x = screen_rect.width/4 - (screen_rect.height - y); x < screen_rect.width - screen_rect.width/4 + (screen_rect.height - y); x++){
            Vec2i screen_coords(x + screen_rect.x, y + screen_rect.y);
            DrawTile(screen_coords, deck_ch, Tile::kShip);
        }
    }

    for(auto cannon : ship->GetRightCannons()){
        Vec2 cannon_pos = elements_positions->find(cannon)->second;
        Vec2i screen_coords(cannon_pos.x + screen_rect.x, cannon_pos.y + screen_rect.y);
        DrawTile(screen_coords, 'C', Tile::kCannon);
    }

    for(auto cannon : ship->GetLeftCannons()){
        Vec2 cannon_pos = elements_positions->find(cannon)->second;
        Vec2i screen_coords(cannon_pos.x + screen_rect.x, cannon_pos.y + screen_rect.y);
        DrawTile(screen_coords, 'C', Tile::kCannon);
    }

    auto masts = ship->GetMasts();
    int mast_ind = 0;
    int mast_width = (screen_rect.width * 1.8) / 2  - 1;
    for(auto mast : masts){
        for(int i = 0; i < mast_width; i++){
            Vec2 rotated_sail_pos = Vec2(i - mast_width / 2, 0).Rotated(mast->GetAngle());
            char ch = i < mast_width / 2 ? 'L' : 'R';
            Vec2 mast_pos = elements_positions->find(mast)->second;
            Vec2i screen_coords(screen_rect.x + mast_pos.x + rotated_sail_pos.x, screen_rect.y + mast_pos.y + rotated_sail_pos.y);
            DrawTile(screen_coords, ch, Tile::kSail);
        }
    }

    for(auto sailor : ship->GetSailors()){
        Vec2 tile_pos;
        SailorState sailor_state = sailor->GetState();
        if(sailor_state == SailorState::kCannon){
            Vec2 cannon_pos = elements_positions->find(sailor->GetOperatedCannon())->second;
            if(sailor->GetOperatedCannon()->GetOwners().first == sailor.get()){
                tile_pos = cannon_pos + Vec2(0,1);
            }else
                tile_pos = cannon_pos - Vec2(0,1);
        }else if(sailor_state == SailorState::kMast){
            auto mast_owners = ship->GetMastDistributor()->masts_owners->at(sailor->GetOperatedMast());
            Vec2 mast_pos = elements_positions->at(sailor->GetOperatedMast());
            int ind = 0;
            for(auto mast_owner : *mast_owners){
                if(mast_owner == sailor.get())
                    break;
                else
                    ind++;
            }
            switch(ind){
                case 0:
                    tile_pos = mast_pos + Vec2(0, -1); break;
                case 1:
                    tile_pos = mast_pos + Vec2(1, 0); break;
                case 2:
                    tile_pos = mast_pos + Vec2(0, 1); break;
                case 3:
                    tile_pos = mast_pos + Vec2(-1, 0); break;
                default:
                    break;
            }
        } else{
            shared_ptr<ShipObject> prev_target = sailor->GetPreviousTarget();
            shared_ptr<ShipObject> next_target = sailor->GetNextTarget();
            float progress = sailor->GetProgress();
            Vec2 prev_target_pos = elements_positions->find(prev_target)->second;
            Vec2 next_target_pos = elements_positions->find(next_target)->second;
            tile_pos = prev_target_pos + (next_target_pos - prev_target_pos) * progress;
        }
        auto sailor_color = GetColor(sailor.get());
        ncurses_util::SetColor(sailor_color, COLOR_BLACK);
        mvaddch(tile_pos.y + screen_rect.y, tile_pos.x + screen_rect.x, 'S');
        ncurses_util::UnsetColor(sailor_color, COLOR_BLACK);
    }
}

void Monitor::DrawCircleIndicator(Vec2i offset, float angle, string label, int size) {
    int arrow_length = (size - 1) / 2;
    int angles = arrow_length * 7;
    mvaddstr(offset.y, offset.x + size/2.f  - label.length() / 2.f, label.c_str());
    Vec2 circle_center(offset.x + arrow_length, offset.y + arrow_length + 1);
    for(int i = 0; i < angles; i++){
        float angle = (float)i / angles * 2 * M_PI;
        Vec2 local_pos = Vec2(arrow_length - 1, 0).Rotated(angle);
        Vec2i screen_coords(circle_center.x + round(local_pos.x), circle_center.y + round(local_pos.y));
        DrawTile(screen_coords, '#', Tile::kGray);
    }
    for(int i = 0; i < arrow_length; i++){
        Vec2 local_pos = Vec2(i, 0).Rotated(angle);
        Vec2i screen_coords(circle_center.x + round(local_pos.x), circle_center.y + round(local_pos.y));
        DrawTile(screen_coords, '#', Tile::kSail);
    }
}

void Monitor::DrawWindDir(Vec2i offset, int size) {
    Vec2 wind_velocity = world->wind->GetVelocity();
    float wind_angle = wind_velocity.Angle();
    DrawCircleIndicator(offset, wind_angle, "Kierunek wiatru", size);
}

void Monitor::DrawShipDir(Vec2i offset, int size, shared_ptr<Ship> ship) {
    DrawCircleIndicator(offset, ship->GetDirection().Angle(), "Kierunek statku", size);
}

void Monitor::DrawSailTarget(Vec2i offset, int size, std::shared_ptr<Ship> ship) {
    float wind_angle = world->wind->GetVelocity().Angle();
    float ship_angle = ship->GetDirection().Angle();
    DrawCircleIndicator(offset, AngleDifference(wind_angle, ship_angle) - M_PI / 2, "Docelowy kat zagli",
                        size);
}

void Monitor::DrawShipInfo(shared_ptr<Ship> ship) {
    DrawDashboard(ship);
    DrawShipDeck(ship, {40, 0, 29, 50});
    DrawWindDir({71, 0}, 18);
    DrawShipDir({71, 20}, 18, ship);
    DrawSailTarget({71, 40}, 18, ship);
    int preview_size = 50;
    DrawWorld({90, 0},
              Rect(ship->GetPosition().x - preview_size/2,
                   ship->GetPosition().y - preview_size/2,
                   preview_size, preview_size)
              );
}

void Monitor::DrawCannonballs(Vec2i screen_offset, Rect world_viewport) {
    lock_guard<mutex> guard(world->cannonballs_mutex);
    for(auto cannonball : world->cannonballs){
        if(cannonball->dead)
            continue;
        Vec2 cannonball_pos = cannonball->GetPos();
        if(cannonball_pos.x >= world_viewport.x && cannonball_pos.x < world_viewport.x + world_viewport.width
           && cannonball_pos.y >= world_viewport.y && cannonball_pos.y < world_viewport.y + world_viewport.height)
        {
            Vec2i screen_coords(screen_offset.x - world_viewport.x + cannonball_pos.x, screen_offset.y - world_viewport.y + cannonball_pos.y);
            DrawTile(screen_coords, 'O', Tile::kCannonball);
        }
    }
}

int Monitor::GetColor(Sailor *sailor) {
    if(sailors_assigned_colors.find(sailor) == sailors_assigned_colors.end()){
        sailors_assigned_colors[sailor] = color_id++;
    }
    int color_ind = sailors_assigned_colors.at(sailor);
    return kSailorsColors.at(color_ind % kSailorsColors.size());
}

void Monitor::NextShip() {
    current_ship_ind++;
    if(current_ship_ind >= world->ships.size())
        current_ship_ind = 0;
}

void Monitor::PrevShip() {
    current_ship_ind--;
    if(current_ship_ind < 0)
        current_ship_ind = world->ships.size() - 1;
}

void Monitor::ChangeDisplayMode() {
    if(display_mode == MonitorDisplayMode::kMap)
        display_mode = MonitorDisplayMode::kDashboard;
    else
        display_mode = MonitorDisplayMode::kMap;
}
