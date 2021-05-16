#include <iostream>
#include "World.h"
#include "Monitor.h"
#include <memory>
#include <ncurses.h>
#include "Vec2.h"
#include "Ship.h"

using namespace std;

int main(int argc, char *argv[]) {
    int seed = 1;
    int world_width = 160, world_height = 70;
    try{
        if(argc > 1)
            seed = stoi(argv[1]);
        if(argc > 2)
            world_width = stoi(argv[2]);
        if(argc > 3)
            world_height = stoi(argv[3]);
    }catch(invalid_argument ex){}
    shared_ptr<World> world = make_shared<World>(world_width, world_height, seed);
    world->AddShip(make_shared<Ship>(Vec2(15,16), Vec2(1,0), world));
    world->AddShip(make_shared<Ship>(Vec2(45,15), Vec2(-1, 0), world));
    world->AddShip(make_shared<Ship>(Vec2(75,15), Vec2(1,1), world));
    Monitor monitor(world);
    monitor.Start();

    //while(getchar() != 27);

    endwin();
    return 0;
}
