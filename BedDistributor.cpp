//
// Created by konrad on 18.05.2021.
//

#include "BedDistributor.h"
#include "Sailor.h"

using namespace std;

BedDistributor::BedDistributor(std::shared_ptr<std::vector<sp<bool>>> beds) {
    this->beds = beds;
    free_beds = make_shared<queue<sp<bool>>>();
    bed_owners = make_shared<unordered_map<sp<bool>, sp<Sailor>>>();
    for(auto bed : *beds){
        free_beds->push(bed);
        bed_owners->insert(make_pair(bed, nullptr));
    }

}

sp<bool> BedDistributor::RequestBed(sp<Sailor> sailor) {
    unique_lock<mutex> lock(beds_mutex);
    sp<bool> target_bed;
    if(!free_beds->empty()){
        target_bed = free_beds->front();
    }else{
        c_var_bed_freed.wait(lock, [&]{return !free_beds->empty();});
    }

    bed_owners->at(target_bed) = sailor;
    free_beds->pop();
    lock.unlock();
    return target_bed;
}

void BedDistributor::ReleaseBed(sp<bool> bed, sp<Sailor> sailor) {
    {
        lock_guard<mutex> guard(beds_mutex);
        bed_owners->at(bed) = nullptr;
        free_beds->push(bed);
    }
    c_var_bed_freed.notify_one();
}
