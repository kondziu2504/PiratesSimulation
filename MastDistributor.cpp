//
// Created by konrad on 16.05.2021.
//

#include "MastDistributor.h"
#include "Mast.h"
#include "Sailor.h"

using namespace std;

MastDistributor::MastDistributor(std::shared_ptr<std::vector<std::shared_ptr<Mast>>> masts) {
    this->masts = masts;
    masts_owners = make_shared<map<sp<Mast>, sp<vector<Sailor*>>>>();
    for(shared_ptr<Mast> mast : *masts){
            masts_owners->insert(make_pair(mast, make_shared<vector<Sailor*>>()));
    }
}

shared_ptr<Mast> MastDistributor::RequestMast(Sailor * sailor) {
    unique_lock<mutex> lock(free_masts_mutex);
    shared_ptr<Mast> target_mast = FindFreeMast();;
    if(target_mast == nullptr){
        c_var_mast_freed.wait(lock, [&]{return (target_mast = FindFreeMast()) != nullptr;});
    }

    masts_owners->at(target_mast)->push_back(sailor);
    lock.unlock();
    return target_mast;
}

int MastDistributor::OccupiedMasts() {
    lock_guard<mutex> guard(free_masts_mutex);
    int occupied_masts = 0;
    for(auto pair : *masts_owners){
        if(!pair.second->empty())
            occupied_masts++;
    }
    return occupied_masts;
}

void MastDistributor::ReleaseMast(std::shared_ptr<Mast> mast, Sailor * sailor) {
    {
        lock_guard<mutex> guard(free_masts_mutex);
        auto owners = masts_owners->at(mast);
        auto it = owners->begin();
        while(it != owners->end()){
            if(*it == sailor){
                owners->erase(it);
                break;
            }else{
                ++it;
            }
        }
    }
    c_var_mast_freed.notify_one();
}

sp<Mast> MastDistributor::FindFreeMast() {
    for(const auto& mast_owners_pair : *masts_owners){
        if(mast_owners_pair.second->size() < mast_owners_pair.first->GetMaxSlots()){
            return mast_owners_pair.first;
        }
    }
    return nullptr;
}
