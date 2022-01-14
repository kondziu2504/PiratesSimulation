#include <iostream>
#include "World.h"
#include "Monitor.h"
#include <memory>
#include <ncurses.h>
#include <thread>
#include "Util/Vec2f.h"
#include "Ship/Ship.h"

#define KEY_ESC 27

using namespace std;

void ReadWorldSettingsFromArgs(int argc, char *argv[], unsigned int & seed, int & world_width, int & world_height);
void SpawnRandomShips(int num_of_ships, World * world);
void SpawnDebugShips(World * world);
void HandleKeyboardInput(World * world, Monitor * monitor);

int main(int argc, char *argv[]) {
    srand(time(nullptr));

    ncurses_util::Initialize();

    unsigned int seed = 1;
    int world_width = 130, world_height = 60;
    ReadWorldSettingsFromArgs(argc, argv, seed, world_width, world_height);

    World world(world_width, world_height, seed);
    SpawnRandomShips(5, &world);

    world.Start();

    Monitor monitor(&world);
    monitor.Start();

    HandleKeyboardInput(&world, &monitor);

    return 0;
}

void ReadWorldSettingsFromArgs(int argc, char *argv[], unsigned int & seed, int & world_width, int & world_height) {
    try{
        if(argc > 1)
            seed = stoi(argv[1]);
        if(argc > 2)
            world_width = stoi(argv[2]);
        if(argc > 3)
            world_height = stoi(argv[3]);
    }catch(invalid_argument ex){}
}

void SpawnRandomShips(int num_of_ships, World * world) {
    for(int i = 0; i < num_of_ships; i++)
        world->AddRandomShip();
}

void SpawnDebugShips(World * world) {
    world->AddShip(make_shared<Ship>(Vec2f(55,18), Vec2f(1, 0), 24, 3, 3, world));
    world->AddShip(make_shared<Ship>(Vec2f(95,15), Vec2f(-1,0), 5, 2, 2, world));
}

void HandleKeyboardInput(World * world, Monitor * monitor) {
    while(true){
        int key = getch();
        if(key == ' '){
            monitor->ChangeDisplayMode();
        }else if(key == 'a'){
            monitor->PrevShip();
        }else if(key == 'd'){
            monitor->NextShip();
        }else if(key == KEY_ESC){
            world->RequestStop();
            world->WaitUntilStopped();
            monitor->RequestStop();
            monitor->WaitUntilStopped();
            break;
        }
    }
}