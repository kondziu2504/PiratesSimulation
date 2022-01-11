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
#include "NcursesUtil.h"
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
            }else{
                DrawTile(screen_coords, ';', Tile::kGray);
            }
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
            if(texture[i][j] != ' '){
                float length = texture[i].length();
                float width = texture.size();
                Vec2 rotatedLocal = Vec2(
                        j - length / 2,
                        i - width / 2).Rotated(ship_dir.Angle());
                Vec2 tile_pos = Vec2((int)ship_pos.x + rotatedLocal.x, (int)ship_pos.y + rotatedLocal.y);
                if(tile_pos.x >= world_viewport.x && tile_pos.x < world_viewport.x + world_viewport.width
                   && tile_pos.y >= world_viewport.y && tile_pos.y < world_viewport.y + world_viewport.height)
                {
                    DrawTile(
                            Vec2i(tile_pos.x + screen_offset.x - world_viewport.x, tile_pos.y + screen_offset.y - world_viewport.y),
                            texture[i][j],
                            ship->GetState() != ShipState::kSinking && ship->GetState() != ShipState::kDestroyed ? Tile::kShip : Tile::kDestroyed);
                }
            }
        }
    }

    //DrawTile(ship_pos.y, ship_pos.x, '#', Tile::kShip);
}

void Monitor::DrawShips(Vec2i offset, Rect world_viewport) {
    lock_guard<mutex> guard(world->ships_mutex);
    for(shared_ptr<Ship> ship : world->ships){
        DrawShip(ship, offset, world_viewport);
    }
}



void Monitor::DrawDashboard(shared_ptr <Ship> ship) {
    int line = 0;
    int indentation = 0;
    indentation = 0;
    string health = ship->GetState() == ShipState::kDestroyed ? "Zniszczony" : "Zdrowie: " + to_string(ship->GetHP());
    string header = "Statek(" + health + ")";
    mvaddstr(line++, 0, header.c_str());
    indentation += 2;
    mvaddstr(line++, indentation, "Marynarze:");
    {
        indentation += 2;
        int sailor_index = 0;
        for(auto sailor : ship->GetSailors()){
            string state_text = "";
            switch (sailor->GetState()) {
                case SailorState::kResting:
                    state_text = "Odpoczywa";
                    break;
                case SailorState::kMast:
                    state_text = "Obsluguje maszt";
                    break;
                case SailorState::kStairs:
                    state_text = "Korzysta ze schodow";
                    break;
                case SailorState::kWalking:
                    state_text = "Idzie";
                    break;
                case SailorState::kWaitingStairs:
                    state_text = "Czeka na schody";
                    break;
                case SailorState::kCannon:
                    state_text = "Obsluguje armate";
                    break;
                case SailorState::kWaitingCannon:
                    state_text = "Czeka na armate";
                    break;
                case SailorState::kStanding:
                    state_text = "Stoi";
                    break;
                case SailorState::kWaitingMast:
                    state_text = "Czeka na maszt";
                    break;
                case SailorState::kDead:
                    state_text = "Martwy";
                    break;
            }
            string sailor_text = "Marynarz " + to_string(sailor_index) + " " + state_text;
            auto sailor_color = GetColor(sailor.get());
            SetColor(sailor_color, 0);
            mvaddstr(line++, indentation, sailor_text.c_str());
            UnsetColor(sailor_color, 0);
            sailor_index++;
        }
    }
    indentation -= 2;
    mvaddstr(line++, indentation, "Maszty:");
    int mast_index = 0;
    indentation += 2;
    lock_guard<mutex> guard(ship->GetMastDistributor()->free_masts_mutex);
    for(auto mast_owners_pair : *ship->GetMastDistributor()->masts_owners){
        bool max_owners = mast_owners_pair.second->size() == mast_owners_pair.first->GetMaxSlots();
        string mast_text =
                "Maszt " + to_string(mast_index) +
                " Obslugiwany przez: " + to_string(mast_owners_pair.second->size()) +
                " " + (max_owners ? "(max)" : "");
        mvaddstr(line++, indentation, mast_text.c_str());
        mast_index++;
    }
    indentation -= 2;
    mvaddstr(line++, indentation, "Armaty:");
    indentation++;
    mvaddstr(line++, indentation, "Lewa strona:");
    indentation++;
    int cannon_ind = 0;
    for(auto cannon : ship->GetLeftCannons()){
        int owners = 0;
        auto cannon_owners = cannon->GetOwners();
        if(cannon_owners.first != nullptr)
            owners++;
        if(cannon_owners.second != nullptr)
            owners++;
        string cannon_message = "Armata " + to_string(cannon_ind) + " Obslugiwana przez: " + to_string(owners) +
                (owners == 2 ? "(Max)" : "");
        mvaddstr(line++, indentation, cannon_message.c_str());
        cannon_ind++;
    }
    indentation--;
    mvaddstr(line++, indentation, "Prawa strona:");
    indentation++;
    cannon_ind = 0;
    for(auto cannon : ship->GetRightCannons()){
        int owners = 0;
        auto cannon_owners = cannon->GetOwners();
        if(cannon_owners.first != nullptr)
            owners++;
        if(cannon_owners.second != nullptr)
            owners++;
        string cannon_message = "Armata " + to_string(cannon_ind) + " Obslugiwana przez: " + to_string(owners) +
                                (owners == 2 ? "(Max)" : "");
        mvaddstr(line++, indentation, cannon_message.c_str());
        cannon_ind++;
    }
    indentation--;
}

void Monitor::DrawShipDeck(shared_ptr <Ship> ship, int x_offset, int y_offset, int width, int height) {
    s_ptr<std::unordered_map<shared_ptr<ShipObject>, Vec2>> elements_positions = make_shared<unordered_map<shared_ptr<ShipObject>, Vec2>>();

    {
        //Generate positions
        auto masts = ship->GetMasts();
        int first_mast_y, last_mast_y;
        if (masts.size() == 1)
            first_mast_y = height / 2;
        else {
            first_mast_y = height / 4;
            last_mast_y = height * 0.9f;
        }

        int mast_y = first_mast_y;
        for (auto mast: masts) {
            elements_positions->insert(make_pair(mast, Vec2(width / 2, mast_y)));
            if (masts.size() > 1) {
                mast_y += (last_mast_y - first_mast_y) / (masts.size() - 1);
            }
        }

        int cannon_y = height / 3;
        auto right_cannons = ship->GetRightCannons();
        int cannon_y_delta = height / (right_cannons.size() + 1);
        for (auto cannon: right_cannons) {
            elements_positions->insert(make_pair(cannon, Vec2(width - 1, cannon_y)));
            cannon_y += cannon_y_delta;
        }

        cannon_y = height / 3;
        auto left_cannons = ship->GetLeftCannons();
        cannon_y_delta = height / (left_cannons.size() + 1);
        for (auto cannon: left_cannons) {
            elements_positions->insert(make_pair(cannon, Vec2(0, cannon_y)));
            cannon_y += cannon_y_delta;
        }

        elements_positions->insert(make_pair(ship->GetRightJunction(), Vec2(width * 0.75, height / 2)));
        elements_positions->insert(make_pair(ship->GetLeftJunction(), Vec2(width * 0.25, height / 2)));
        elements_positions->insert(make_pair(ship->GetRestingPoint(), Vec2(width * 0.75, height * 0.25)));
    }

    char deck_ch = ' ';
    for(int y = 0; y < width/2; y++){
        for(int x = width/2 - y; x <= width/2 + y; x++){
            Vec2i screen_coords(x + x_offset, y + y_offset);
            DrawTile(screen_coords, deck_ch, Tile::kShip);
        }
    }

    for(int y = width/2; y < height - width/4; y++){
        for(int x = 0; x < width; x++){
            Vec2i screen_coords(x + x_offset, y + y_offset);
            DrawTile(screen_coords, deck_ch, Tile::kShip);
        }
    }

    for(int y = height - width/4; y < height; y++){
        for(int x = width/4 - (height - y); x < width - width/4 + (height - y); x++){
            Vec2i screen_coords(x + x_offset, y + y_offset);
            DrawTile(screen_coords, deck_ch, Tile::kShip);
        }
    }

    for(auto cannon : ship->GetRightCannons()){
        Vec2 cannon_pos = elements_positions->find(cannon)->second;
        Vec2i screen_coords(cannon_pos.x + x_offset, cannon_pos.y + y_offset);
        DrawTile(screen_coords, 'C', Tile::kCannon);
    }

    for(auto cannon : ship->GetLeftCannons()){
        Vec2 cannon_pos = elements_positions->find(cannon)->second;
        Vec2i screen_coords(cannon_pos.x + x_offset, cannon_pos.y + y_offset);
        DrawTile(screen_coords, 'C', Tile::kCannon);
    }

    auto masts = ship->GetMasts();
    int mast_ind = 0;
    int mast_width = (width * 1.8) / 2  - 1;
    for(auto mast : masts){
        for(int i = 0; i < mast_width; i++){
            Vec2 rotated_sail_pos = Vec2(i - mast_width / 2, 0).Rotated(mast->GetAngle());
            char ch = i < mast_width / 2 ? 'L' : 'R';
            Vec2 mast_pos = elements_positions->find(mast)->second;
            Vec2i screen_coords(x_offset + mast_pos.x + rotated_sail_pos.x, y_offset + mast_pos.y + rotated_sail_pos.y);
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
        SetColor(sailor_color, COLOR_BLACK);
        mvaddch(tile_pos.y + y_offset, tile_pos.x + x_offset, 'S');
        UnsetColor(sailor_color, COLOR_BLACK);
    }
}

void Monitor::DrawCircleIndicator(int x_offset, int y_offset, float angle, string label, int size) {
    int arrow_length = (size - 1) / 2;
    int angles = arrow_length * 7;
    mvaddstr(y_offset, x_offset + size/2.f  - label.length() / 2.f, label.c_str());
    Vec2 circle_center(x_offset + arrow_length, y_offset + arrow_length + 1);
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

void Monitor::DrawWindDir(int x_offset, int y_offset, int size) {
    Vec2 wind_velocity = world->wind->GetVelocity();
    float wind_angle = wind_velocity.Angle();
    DrawCircleIndicator(x_offset, y_offset, wind_angle, "Kierunek wiatru", size);
}

void Monitor::DrawShipDir(int x_offset, int y_offset, int size, shared_ptr<Ship> ship) {
    DrawCircleIndicator(x_offset, y_offset, ship->GetDirection().Angle(), "Kierunek statku", size);
}

void Monitor::DrawSailTarget(int x_offset, int y_offset, int size, std::shared_ptr<Ship> ship) {
    float wind_angle = world->wind->GetVelocity().Angle();
    float ship_angle = ship->GetDirection().Angle();
    DrawCircleIndicator(x_offset, y_offset, AngleDifference(wind_angle, ship_angle) - M_PI / 2, "Docelowy kat zagli",
                        size);
}

void Monitor::DrawShipInfo(shared_ptr<Ship> ship) {
    DrawDashboard(ship);
    DrawShipDeck(ship, 40, 0, 29, 50);
    DrawWindDir(71, 0, 18);
    DrawShipDir(71, 20, 18, ship);
    DrawSailTarget(71, 40, 18, ship);
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

void Monitor::SetColor(int fg, int bg) {
    attron(COLOR_PAIR(ncurses_util::ColorNum(fg, bg)));
}

void Monitor::UnsetColor(int fg, int bg) {
    attroff(COLOR_PAIR(ncurses_util::ColorNum(fg, bg)));
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
