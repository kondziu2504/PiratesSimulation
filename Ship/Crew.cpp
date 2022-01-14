//
// Created by konrad on 01.01.2022.
//

#include "Crew.h"
#include <thread>

using namespace std;

std::shared_ptr<std::vector<std::shared_ptr<Sailor>>>
Crew::GenerateSailors(int sailors_count, ShipBody *operated_ship, WorldObject *parent) {
    auto generated_sailors = make_shared<vector<shared_ptr<Sailor>>>();
    for(int i = 0; i < sailors_count; i++){
        shared_ptr<Sailor> sailor = make_shared<Sailor>(operated_ship, parent);
        generated_sailors->push_back(sailor);
    }
    return generated_sailors;
}

Crew::Crew(int crew_size, ShipBody *operated_ship, WorldObject * parent) :
        sailors(GenerateSailors(crew_size, operated_ship, parent)){}

std::vector<std::shared_ptr<Sailor>> Crew::GetSailors() {
    return *sailors;
}

void Crew::SetOrders(SailorOrder new_order) {
    for(auto sailor : *sailors)
        sailor->SetCurrentOrder(new_order);
}

void Crew::SetCannonsTarget(WorldObject *target) {
    for(auto sailor : *sailors)
        sailor->SetCannonTarget(target);
}

void Crew::SetUseRightCannons(bool right) {
    for(const auto& sailor : *sailors){
        sailor->SetUseRightCannons(right);
    }
}

void Crew::ThreadFunc(const atomic<bool> &stop_requested) {
    vector<thread> sailors_threads;

    for(auto sailor : *sailors)
        sailor->Start();

    WaitUntilStopRequested();
    for(auto sailor : *sailors)
        sailor->RequestStop();

    for(auto sailor : *sailors)
        sailor->WaitUntilStopped();
}
