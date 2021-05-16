//
// Created by konrad on 16.05.2021.
//

#include "MastDistributor.h"
#include "Mast.h"

using namespace std;

MastDistributor::MastDistributor(std::shared_ptr<std::vector<std::shared_ptr<Mast>>> masts) {
    this->masts = masts;
    free_masts = make_shared<queue<shared_ptr<Mast>>>();
    for(shared_ptr<Mast> mast : *masts){
        free_masts->push(mast);
    }
}

void MastDistributor::NotifySlotFreed(std::shared_ptr<Mast> mast) {

}

shared_ptr<Mast> MastDistributor::RequestMast() {
    shared_ptr<Mast> target_mast;
    unique_lock<mutex> lock(free_masts_mutex);
    if(!free_masts->empty()){
        target_mast = free_masts->front();
    }else{
        c_var_mast_freed.wait(lock, [&]{return !free_masts->empty();});
        target_mast = free_masts->front();
    }
    free_masts->pop();
    lock.unlock();
    return target_mast;
}

int MastDistributor::OccupiedMasts() {
    lock_guard<mutex> guard(free_masts_mutex);
    return masts->size() - free_masts->size();
}

void MastDistributor::ReleaseMast(std::shared_ptr<Mast> mast) {
    lock_guard<mutex> guard(free_masts_mutex);
    free_masts->push(mast);
    c_var_mast_freed.notify_one();
}
