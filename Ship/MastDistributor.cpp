//
// Created by konrad on 16.05.2021.
//

#include "MastDistributor.h"
#include "Mast.h"
#include "Sailor.h"

using namespace std;

MastDistributor::MastDistributor(const std::vector<Mast *> & masts) {
    for(const auto mast : masts){
            masts_map_owners.insert(make_pair(mast, move(vector<Sailor*>())));
    }
}

Mast * MastDistributor::RequestMast(Sailor * sailor) {
    unique_lock<mutex> lock(masts_owners_mutex);
    Mast * target_mast = FindFreeMast();
    if(target_mast == nullptr){
        c_var_mast_freed.wait(lock, [&]{return (target_mast = FindFreeMast()) != nullptr;});
    }

    masts_map_owners.at(target_mast).push_back(sailor);
    lock.unlock();
    return target_mast;
}

void MastDistributor::ReleaseMast(Mast * mast, Sailor * sailor) {
    {
        lock_guard<mutex> guard(masts_owners_mutex);
        auto & owners = masts_map_owners.at(mast);
        auto it = owners.begin();
        while(it != owners.end()){
            if(*it == sailor){
                owners.erase(it);
                break;
            }else{
                ++it;
            }
        }
    }
    c_var_mast_freed.notify_one();
}

Mast * MastDistributor::FindFreeMast() const {
    for(const auto& mast_owners_pair : masts_map_owners){
        if(mast_owners_pair.second.size() < mast_owners_pair.first->GetMaxSlots()){
            return mast_owners_pair.first;
        }
    }
    return nullptr;
}

std::map<Mast *, std::vector<Sailor *>> MastDistributor::GetMastsOwners() const {
    lock_guard<mutex> guard(masts_owners_mutex);
    return masts_map_owners;
}
