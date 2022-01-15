//
// Created by konrad on 01.01.2022.
//

#include "Crew.h"
#include <thread>

using namespace std;

std::vector<std::unique_ptr<Sailor>> Crew::GenerateSailors(int sailors_count, ShipBody *operated_ship, WorldObject *parent) {
    auto generated_sailors = vector<unique_ptr<Sailor>>();
    generated_sailors.reserve(sailors_count);
    for(int i = 0; i < sailors_count; i++){
        unique_ptr<Sailor> sailor = make_unique<Sailor>(operated_ship, parent);
        generated_sailors.push_back(move(sailor));
    }
    return move(generated_sailors);
}

Crew::Crew(int crew_size, ShipBody *operated_ship, WorldObject * parent) :
        sailors(GenerateSailors(crew_size, operated_ship, parent)){}

std::vector<Sailor *> Crew::GetSailors() const {
    return MapToRawPointers(sailors);
}

void Crew::SetOrders(SailorOrder new_order) {
    for(const auto& sailor : sailors)
        sailor->SetCurrentOrder(new_order);
}

void Crew::SetCannonsTarget(const weak_ptr<WorldObject> & target) {
    for(const auto& sailor : sailors)
        sailor->SetCannonTarget(target);
}

void Crew::SetUseRightCannons(bool right) {
    for(const auto& sailor : sailors){
        sailor->SetUseRightCannons(right);
    }
}

void Crew::ThreadFunc(const atomic<bool> &stop_requested) {
    vector<thread> sailors_threads;

    for(const auto& sailor : sailors)
        sailor->Start();

    WaitUntilStopRequested();
    for(const auto& sailor : sailors)
        sailor->RequestStop();

    for(const auto& sailor : sailors)
        sailor->WaitUntilStopped();
}
