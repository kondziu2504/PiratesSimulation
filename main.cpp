#include <iostream>
#include "World.h"
#include "Monitor.h"
#include <memory>
#include <ncurses.h>

using namespace std;

int main(int argc, char *argv[]) {
    int seed = 1;
    try{
        if(argc > 1)
            seed = stoi(argv[1]);
    }catch(invalid_argument ex){}
    shared_ptr<World> world = make_shared<World>(190, 50, seed);
    world->AddShip(make_shared<Ship>(Vec2(15,15), world));
    Monitor monitor(world);
    monitor.Start();

    getchar();
    int g = 0;

    endwin();
    return 0;
}
