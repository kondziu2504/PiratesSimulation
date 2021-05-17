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

    init_color(COLOR_BLACK, 0, 0, 0);
    init_color(COLOR_RED, 1000, 350, 350);
    init_color(COLOR_CYAN, 250, 1000, 1000);
    init_color(COLOR_YELLOW, 1000, 1000, 250);
    init_color(COLOR_GREEN, 250, 1000, 250);
    init_color(COLOR_MAGENTA, 1000, 500, 1000);
    init_color(COLOR_BROWN, 700, 350, 0);
    init_color(COLOR_GRAY, 300, 300, 300);

    init_pair(static_cast<short>(Tile::kWater), COLOR_CYAN, COLOR_BLUE );
    init_pair(static_cast<short>(Tile::kShip), COLOR_YELLOW, COLOR_BROWN );
    init_pair(static_cast<short>(Tile::kLand), COLOR_YELLOW, COLOR_GREEN);
    init_pair(static_cast<short>(Tile::kSail), COLOR_BLACK, COLOR_WHITE);
    init_pair(static_cast<short>(Tile::kGray), COLOR_GRAY, COLOR_GRAY);
}

void Monitor::Stop() {

}

void Monitor::Update() {
    clear();
    {
        lock_guard<mutex> guard(display_mode_mutex);
        if(display_mode == MonitorDisplayMode::kMap){
            DrawWorld();
        }else{
            DrawDashboard();
            DrawShipDeck(40, 0, 30, 60);
            DrawWindDir(71, 0, 18);
            DrawShipDir(71, 20, 18, world->ships[0]);
            DrawSailTarget(71, 40, 18, world->ships[0]);
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

void Monitor::DrawMap() {
    for(int x = 0; x < world->width; x++){
        for(int y = 0; y < world->height; y++){
            if(world->map[y * world->width + x])
                DrawTile(y, x, ',', Tile::kLand);
            else
                DrawTile(y, x, ';', Tile::kWater);
        }
    }
}

void Monitor::DrawTile(int y, int x, char ch, Tile tile) {
    attron(COLOR_PAIR((int)tile));
    mvaddch(y, x, ch);
    attroff(COLOR_PAIR((int)tile));
}

void Monitor::DrawWorld() {
    DrawMap();
    DrawShips();
}

void Monitor::DrawShip(shared_ptr<Ship> ship) {
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
                DrawTile(
                        (int)ship_pos.y + rotatedLocal.y,
                        (int)ship_pos.x + rotatedLocal.x,
                        texture[i][j], Tile::kShip);
            }
        }
    }

    //DrawTile(ship_pos.y, ship_pos.x, '#', Tile::kShip);
}

void Monitor::DrawShips() {
    lock_guard<mutex> guard(world->shipsMutex);
    for(shared_ptr<Ship> ship : world->ships){
        DrawShip(ship);
    }
}

void Monitor::DrawDashboard() {
    int ship_index = 0;
    int line = 0;
    int indentation = 0;
    for(auto ship : world->ships){
        indentation = 0;
        string header = "Statek " + to_string(ship_index);
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

        ship_index++;
    }
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
        }
    }
}

void Monitor::DrawShipDeck(int x_offset, int y_offset, int width, int height) {
    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            DrawTile(y + y_offset, x + x_offset, ',', Tile::kShip);
        }
    }

    int ship_id = 0;

    lock_guard<mutex> guard(world->shipsMutex);
    auto masts = world->ships[ship_id]->masts;
    int first_mast_y, last_mast_y;
    if(masts->size() == 1)
        first_mast_y = height / 2;
    else{
        first_mast_y = height / 4;
        last_mast_y = height - first_mast_y;
    }
    int mast_ind = 0;
    int mast_y = first_mast_y;
    int mast_width = 13;
    for(auto mast : *masts){
        for(int i = 0; i < mast_width; i++){
            Vec2 rotated_sail_pos = Vec2(i - mast_width / 2, 0).Rotate(mast->GetAngle());
            char ch = i < mast_width / 2 ? 'L' : 'R';
            DrawTile(y_offset + mast_y + rotated_sail_pos.y,  rotated_sail_pos.x + x_offset + width / 2, ch, Tile::kSail);
        }
        if(masts->size() > 1){
            mast_y += (last_mast_y - first_mast_y) / (masts->size() - 1);
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
