//
// Created by konrad on 14.05.2021.
//

#include <ncurses.h>
#include <unistd.h>
#include "Monitor.h"
#include <thread>
#include <iostream>
#include <cmath>
#include "Vec2.h"
#include "Ship.h"
#include "Sailor.h"
#include "MastDistributor.h"
#include "Mast.h"
#include "Wind.h"
#include "Util.h"

using namespace std;

#define COLOR_BROWN 8
#define COLOR_GRAY 9

Monitor::Monitor(shared_ptr<World> world) {
    this->world = world;

    visualized_ships = make_shared<unordered_set<sp<Ship>>>();
    elements_positions = make_shared<unordered_map<sp<void>, Vec2>>();
}


void Monitor::Start() {
    Initialize();
    thread monitorThread(&Monitor::UpdateThread, this);
    //monitorThread.detach();
    thread inputThread(&Monitor::InputThread, this);
    inputThread.detach();
    monitorThread.join();
}

void Monitor::Initialize() {
    initscr();
    cbreak();
    noecho();
    clear();
    refresh();
    curs_set(0);

    if(!has_colors())
    {
        endwin();
        printf("No color support");
        exit(1);
    }

    start_color();

    init_color(COLOR_WHITE, 1000, 1000, 1000);
    init_color(COLOR_BLACK, 0, 0, 0);
    init_color(COLOR_RED, 1000, 350, 350);
    init_color(COLOR_CYAN, 250, 1000, 1000);
    init_color(COLOR_YELLOW, 1000, 1000, 250);
    init_color(COLOR_GREEN, 250, 1000, 250);
    init_color(COLOR_MAGENTA, 1000, 500, 1000);
    init_color(COLOR_BROWN, 500, 275, 0);
    init_color(COLOR_GRAY, 300, 300, 300);

    init_pair(static_cast<short>(Tile::kWater), COLOR_CYAN, COLOR_BLUE );
    init_pair(static_cast<short>(Tile::kShip), COLOR_YELLOW, COLOR_BROWN );
    init_pair(static_cast<short>(Tile::kLand), COLOR_YELLOW, COLOR_GREEN);
    init_pair(static_cast<short>(Tile::kSail), COLOR_WHITE, COLOR_BROWN);
    init_pair(static_cast<short>(Tile::kGray), COLOR_GRAY, COLOR_GRAY);
    init_pair(static_cast<short>(Tile::kSailor), COLOR_YELLOW, COLOR_BLACK);
    init_pair(static_cast<short>(Tile::kStairs), COLOR_YELLOW, COLOR_BLACK);
}

void Monitor::Stop() {

}

void Monitor::Update() {
    clear();
    {
        lock_guard<mutex> guard(display_mode_mutex);
        if(display_mode == MonitorDisplayMode::kMap){
            DrawWorld(0, 0, 0, 0, 0, 0);
        }else{
            lock_guard<mutex> guard(current_ship_mutex);
            DrawShipInfo(current_ship_ind);
        }
    }
    refresh();
}

[[noreturn]] void Monitor::UpdateThread() {
    while(true)
    {
        Update();
        usleep(100000);
    }
}

void
Monitor::DrawMap(int x_offset, int y_offset, int x_viewport, int y_viewport, int viewport_width, int viewport_height) {
    for(int x = 0; x < viewport_width; x++){
        int map_x = x + x_viewport;
        for(int y = 0; y < viewport_height; y++){
            int map_y = y + y_viewport;
            if(map_x >= 0 && map_x < world->width && map_y >= 0 && map_y < world->height){
                if(world->map[map_y * world->width + map_x])
                    DrawTile(y_offset + y, x_offset + x, ',', Tile::kLand);
                else
                    DrawTile(y_offset + y, x_offset + x, ';', Tile::kWater);
            }else{
                DrawTile(y_offset + y, x_offset + x, ';', Tile::kGray);
            }
        }
    }
}

void Monitor::DrawTile(int y, int x, char ch, Tile tile) {
    attron(COLOR_PAIR((int)tile));
    mvaddch(y, x, ch);
    attroff(COLOR_PAIR((int)tile));
}

void Monitor::DrawWorld(int x_offset, int y_offset, int x_viewport, int y_viewport, int viewport_width, int viewport_height) {
    if(viewport_width == 0)
        viewport_width = world->width;
    if(viewport_height == 0)
        viewport_height = world->height;

    DrawMap(x_offset, y_offset, x_viewport, y_viewport, viewport_width, viewport_height);
    DrawShips(x_offset, y_offset, x_viewport, y_viewport, viewport_width, viewport_height);
}

void Monitor::DrawShip(shared_ptr<Ship> ship, int x_offset, int y_offset, int x_viewport, int y_viewport, int viewport_width, int viewport_height) {
    Vec2 ship_pos = ship->GetPos();
    Vec2 ship_dir = ship->GetDir();

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
                        i - width / 2).Rotate( ship_dir.Angle());
                Vec2 tile_pos = Vec2((int)ship_pos.x + rotatedLocal.x, (int)ship_pos.y + rotatedLocal.y);
                if(tile_pos.x >= x_viewport && tile_pos.x < x_viewport + viewport_width
                    && tile_pos.y >= y_viewport && tile_pos.y < y_viewport + viewport_height)
                {
                    DrawTile(
                            tile_pos.y + y_offset - y_viewport,
                            tile_pos.x + x_offset - x_viewport,
                            texture[i][j], Tile::kShip);
                }
            }
        }
    }

    //DrawTile(ship_pos.y, ship_pos.x, '#', Tile::kShip);
}

void Monitor::DrawShips(int x_offset, int y_offset, int x_viewport, int y_viewport, int viewport_width, int viewport_height) {
    lock_guard<mutex> guard(world->shipsMutex);
    for(shared_ptr<Ship> ship : world->ships){
        DrawShip(ship, x_offset, y_offset, x_viewport, y_viewport, viewport_width, viewport_height);
    }
}

void Monitor::DrawDashboard(shared_ptr <Ship> ship) {
    int line = 0;
    int indentation = 0;
    indentation = 0;
    string header = "Statek ";
    mvaddstr(line++, 0, header.c_str());
    indentation += 2;
    mvaddstr(line++, indentation, "Marynarze:");
    {
        indentation += 2;
        int sailor_index = 0;
        lock_guard<mutex> guard(ship->sailors_mutex);
        for(auto sailor : *ship->sailors){
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
            }
            string sailor_text = "Marynarz " + to_string(sailor_index) + " " + state_text;
            mvaddstr(line++, indentation, sailor_text.c_str());
            sailor_index++;
        }
    }
    indentation -= 2;
    mvaddstr(line++, indentation, "Maszty:");
    int mast_index = 0;
    indentation += 2;
    lock_guard<mutex> guard(ship->distributor->free_masts_mutex);
    for(auto mast_owners_pair : *ship->distributor->masts_owners){
        bool max_owners = mast_owners_pair.second->size() == mast_owners_pair.first->GetMaxSlots();
        string mast_text =
                "Maszt " + to_string(mast_index) +
                " Obslugiwany przez: " + to_string(mast_owners_pair.second->size()) +
                " " + (max_owners ? "(max)" : "");
        mvaddstr(line++, indentation, mast_text.c_str());
        mast_index++;
    }
    indentation -= 2;
}

[[noreturn]] void Monitor::InputThread() {
    while(true){
        char key = getchar();
        if(key == ' '){
             lock_guard<mutex> guard(display_mode_mutex);
            if(display_mode == MonitorDisplayMode::kMap)
                display_mode = MonitorDisplayMode::kDashboard;
            else
                display_mode = MonitorDisplayMode::kMap;
        }else if(key == 'a'){
            lock_guard<mutex> guard(current_ship_mutex);
            current_ship_ind--;
            if(current_ship_ind < 0)
                current_ship_ind = world->ships.size() - 1;
        }else if(key == 'd'){
            lock_guard<mutex> guard(current_ship_mutex);
            current_ship_ind++;
            if(current_ship_ind >= world->ships.size())
                current_ship_ind = 0;
        }
    }
}

void Monitor::DrawShipDeck(shared_ptr <Ship> ship, int x_offset, int y_offset, int width, int height) {

    if(visualized_ships->find(ship) == visualized_ships->end()){
        visualized_ships->insert(ship);
        //Generate positions
        auto masts = ship->masts;
        int first_mast_y, last_mast_y;
        if(masts->size() == 1)
            first_mast_y = height / 2;
        else{
            first_mast_y = height / 3;
            last_mast_y = height - first_mast_y;
        }

        int mast_y = first_mast_y;
        for(auto mast : *masts){
            elements_positions->insert(make_pair(mast, Vec2(width/2, mast_y)));
            if(masts->size() > 1){
                mast_y += (last_mast_y - first_mast_y) / (masts->size() - 1);
            }
        }

        elements_positions->insert(make_pair(ship->distributor, Vec2(width * 0.75, height/2)));
        elements_positions->insert(make_pair(ship->stairs_mutex, Vec2(width/2, width/2)));
        elements_positions->insert(make_pair(nullptr, Vec2(width * 0.75, height * 0.25)));
    }

    char deck_ch = ' ';
    for(int y = 0; y < width/2; y++){
        for(int x = width/2 - y; x < width/2 + y; x++){
            DrawTile(y + y_offset, x + x_offset, deck_ch, Tile::kShip);
        }
    }

    for(int y = width/2; y < height - width/4; y++){
        for(int x = 0; x < width; x++){
            DrawTile(y + y_offset, x + x_offset, deck_ch, Tile::kShip);
        }
    }

    for(int y = height - width/4; y < height; y++){
        for(int x = width/4 - (height - y); x < width - width/4 + (height - y); x++){
            DrawTile(y + y_offset, x + x_offset, deck_ch, Tile::kShip);
        }
    }

    Vec2 stairs_pos = elements_positions->find(ship->stairs_mutex)->second;
    DrawTile(stairs_pos.y + y_offset, stairs_pos.x + x_offset, '=', Tile::kStairs);

    auto masts = ship->masts;
    int mast_ind = 0;
    int mast_width = (width * 1.8) / 2  - 1;
    for(auto mast : *masts){
        for(int i = 0; i < mast_width; i++){
            Vec2 rotated_sail_pos = Vec2(i - mast_width / 2, 0).Rotate(mast->GetAngle());
            char ch = i < mast_width / 2 ? 'L' : 'R';
            Vec2 mast_pos = elements_positions->find(mast)->second;
            DrawTile(y_offset + mast_pos.y + rotated_sail_pos.y,  rotated_sail_pos.x + x_offset + mast_pos.x, ch, Tile::kSail);
        }
    }

    for(auto sailor : *ship->sailors){
        if(sailor->IsUpperDeck()){
            shared_ptr<void> prev_target = sailor->GetPreviousTarget();
            shared_ptr<void> next_target = sailor->GetNextTarget();
            float progress = sailor->GetProgress();
            Vec2 prev_target_pos = elements_positions->find(prev_target)->second;
            Vec2 next_target_pos = elements_positions->find(next_target)->second;
            Vec2 tile_pos = prev_target_pos + (next_target_pos - prev_target_pos) * progress;
            DrawTile(tile_pos.y + y_offset, tile_pos.x + x_offset, 'S', Tile::kSailor);
        }
    }
}

void Monitor::DrawCircleIndicator(int x_offset, int y_offset, float angle, string label, int size) {
    int arrow_length = (size - 1) / 2;
    int angles = arrow_length * 7;
    mvaddstr(y_offset, x_offset + size/2.f  - label.length() / 2.f, label.c_str());
    Vec2 circle_center(x_offset + arrow_length, y_offset + arrow_length + 1);
    for(int i = 0; i < angles; i++){
        float angle = (float)i / angles * 2 * M_PI;
        Vec2 local_pos = Vec2(arrow_length - 1, 0).Rotate(angle);
        DrawTile(circle_center.y + round(local_pos.y), circle_center.x + round(local_pos.x), '#', Tile::kGray);
    }
    for(int i = 0; i < arrow_length; i++){
        Vec2 local_pos = Vec2(i, 0).Rotate(angle);
        DrawTile(circle_center.y + round(local_pos.y), circle_center.x + round(local_pos.x), '#', Tile::kSail);
    }
}

void Monitor::DrawWindDir(int x_offset, int y_offset, int size) {
    Vec2 wind_velocity = world->wind->GetVelocity();
    float wind_angle = wind_velocity.Angle();
    DrawCircleIndicator(x_offset, y_offset, wind_angle, "Kierunek wiatru", size);
}

void Monitor::DrawShipDir(int x_offset, int y_offset, int size, shared_ptr<Ship> ship) {
    DrawCircleIndicator(x_offset, y_offset, ship->GetDir().Angle(), "Kierunek statku", size);
}

void Monitor::DrawSailTarget(int x_offset, int y_offset, int size, std::shared_ptr<Ship> ship) {
    float wind_angle = world->wind->GetVelocity().Angle();
    float ship_angle = ship->GetDir().Angle();
    DrawCircleIndicator(x_offset, y_offset, AngleDifference(wind_angle, ship_angle) - M_PI / 2, "Docelowy kat zagli",
                        size);
}

void Monitor::DrawShipInfo(int ship_ind) {
    auto ship = world->ships[ship_ind];
    DrawDashboard(ship);
    DrawShipDeck(ship, 40, 0, 20, 40);
    DrawWindDir(71, 0, 18);
    DrawShipDir(71, 20, 18, ship);
    DrawSailTarget(71, 40, 18, ship);
    int preview_size = 50;
    DrawWorld(90, 0,
              ship->GetPos().x - preview_size/2,
              ship->GetPos().y - preview_size/2,
              preview_size, preview_size);
}
