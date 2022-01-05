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

void Crew::Start() {
    vector<thread> sailors_threads;

    for(auto sailor : *sailors)
        sailors_threads.emplace_back(&Sailor::Start, sailor);

    for(thread & sailor_thread : sailors_threads)
        sailor_thread.join();
}

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

void Crew::Kill() {
    for(const auto& sailor : *sailors){
        sailor->Kill();
    }
}
