#include <iostream>
#include "World.h"
#include "Monitor.h"
#include <memory>
#include <ncurses.h>
#include "Vec2.h"
#include "Ship.h"

using namespace std;

int main(int argc, char *argv[]) {
    srand(time(NULL));
    int seed = 1;
    int world_width = 130, world_height = 60;
    try{
        if(argc > 1)
            seed = stoi(argv[1]);
        if(argc > 2)
            world_width = stoi(argv[2]);
        if(argc > 3)
            world_height = stoi(argv[3]);
    }catch(invalid_argument ex){}
    shared_ptr<World> world = make_shared<World>(world_width, world_height, seed);
    world->AddShip(make_shared<Ship>(Vec2(15,16), Vec2(1,0), 5, 2, 2, world.get()));
    world->AddShip(make_shared<Ship>(Vec2(55,18), Vec2(1, 0), 24, 3, 3, world.get()));
    world->AddShip(make_shared<Ship>(Vec2(95,15), Vec2(-1,0), 5, 2, 2, world.get()));
    for(int i = 0; i < 4; i++)
        world->GenerateShip();
    Monitor monitor(world);
    monitor.Start();
    //while(true);

    //while(getchar() != 27);

    endwin();
    return 0;
}
