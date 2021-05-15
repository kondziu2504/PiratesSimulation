//
// Created by konrad on 14.05.2021.
//

#include <ncurses.h>
#include <unistd.h>
#include "Monitor.h"
#include <thread>
#include <iostream>

using namespace std;

Monitor::Monitor(shared_ptr<World> world) {
    this->world = world;
}


void Monitor::Start() {
    Initialize();
    thread monitorThread(&Monitor::UpdateThread, this);
    monitorThread.detach();
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

    init_pair(static_cast<short>(Tile::kWater), COLOR_CYAN, COLOR_BLUE );
    init_pair(static_cast<short>(Tile::kShip), COLOR_YELLOW, COLOR_BLUE );
    init_pair(static_cast<short>(Tile::kLand), COLOR_YELLOW, COLOR_GREEN);
}

void Monitor::Stop() {

}

void Monitor::Update() {
    DrawWorld();
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
    Pos ship_pos = ship->GetPos();
    DrawTile(ship_pos.y, ship_pos.x, '#', Tile::kShip);
}

void Monitor::DrawShips() {
    lock_guard<mutex> guard(world->shipsMutex);
    for(shared_ptr<Ship> ship : world->ships){
        DrawShip(ship);
    }
}
