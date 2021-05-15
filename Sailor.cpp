//
// Created by konrad on 16.05.2021.
//

#include <thread>
#include <unistd.h>
#include "Sailor.h"

using namespace std;

void Sailor::Start() {
    thread sailor_thread(&Sailor::ThreadFun, this);
}

[[noreturn]] void Sailor::ThreadFun() {
    while(true){


    }
}

void Sailor::GoOperateMast() {
    usleep(1000000);
    //for(int i = 0; i < ship)
}

void Sailor::GoRest() {

}

Sailor::Sailor(std::shared_ptr<Ship> ship) {
    this->ship = ship;
}
