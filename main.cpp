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
    try{
        if(argc > 1)
            seed = stoi(argv[1]);
    }catch(invalid_argument ex){}
    shared_ptr<World> world = make_shared<World>(180, 40, seed);
    world->AddShip(make_shared<Ship>(Vec2(15,16), Vec2(1,0), world));
    world->AddShip(make_shared<Ship>(Vec2(45,15), Vec2(-1, 0), world));
    world->AddShip(make_shared<Ship>(Vec2(75,15), Vec2(1,1), world));
    Monitor monitor(world);
    monitor.Start();

    getchar();
    int g = 0;

    endwin();
    return 0;
}
