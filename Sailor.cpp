//
// Created by konrad on 16.05.2021.
//

#include <thread>
#include <unistd.h>
#include "Sailor.h"
#include "Mast.h"
#include "Ship.h"
#include "MastDistributor.h"

using namespace std;

void Sailor::Start() {
    thread sailor_thread(&Sailor::ThreadFun, this);
    sailor_thread.detach();
}

[[noreturn]] void Sailor::ThreadFun() {
    while(true){
        usleep(1000000);
        shared_ptr<Mast> occupied_mast = ship->distributor->RequestMast();
        usleep(1000000);
        ship->distributor->ReleaseMast(occupied_mast);
    }
}

void Sailor::GoOperateMast() {
    usleep(1000000);
    ship->distributor->RequestMast();
}

void Sailor::GoRest() {

}

Sailor::Sailor(Ship * ship) {
    this->ship = ship;
}
