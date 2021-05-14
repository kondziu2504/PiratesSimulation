#include <iostream>
#include "World.h"
#include "Monitor.h"
#include <memory>
#include <ncurses.h>

using namespace std;

int main() {
    World world(20, 20);
    world.AddShip(make_shared<Ship>(Pos(3,3)));
    Monitor monitor(world);
    monitor.Start();

    getchar();
    int g = 0;

    endwin();
    return 0;
}
