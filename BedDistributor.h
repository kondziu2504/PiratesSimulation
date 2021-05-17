//
// Created by konrad on 18.05.2021.
//

#ifndef PIRATESSIMULATION_BEDDISTRIBUTOR_H
#define PIRATESSIMULATION_BEDDISTRIBUTOR_H


#include <vector>
#include <memory>
#include <queue>
#include <unordered_map>
#include <mutex>
#include <condition_variable>

class Sailor;

template<typename T>
using sp = std::shared_ptr<T>;

class BedDistributor {
    sp<std::vector<sp<bool>>> beds;
    sp<std::queue<sp<bool>>> free_beds;
    sp<std::unordered_map<sp<bool>,sp<Sailor>>> bed_owners;
    std::mutex beds_mutex;
    std::condition_variable c_var_bed_freed;
public:
    explicit BedDistributor(sp<std::vector<sp<bool>>> beds);
    sp<bool> RequestBed(sp<Sailor> sailor);
    void ReleaseBed(sp<bool> bed, sp<Sailor> sailor);
};


#endif //PIRATESSIMULATION_BEDDISTRIBUTOR_H
