//
// Created by konrad on 16.05.2021.
//

#ifndef PIRATESSIMULATION_MASTDISTRIBUTOR_H
#define PIRATESSIMULATION_MASTDISTRIBUTOR_H

#include <memory>
#include <vector>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <map>

class Mast;
class Sailor;

template<typename T>
using sp = std::shared_ptr<T>;

class MastDistributor {
    std::vector<std::shared_ptr<Mast>> masts;


    std::condition_variable c_var_mast_freed;
    sp<Mast> FindFreeMast();

public:
    std::mutex free_masts_mutex;
    sp<std::map<sp<Mast>, sp<std::vector<Sailor*>>>> masts_owners;
    explicit MastDistributor(std::vector<std::shared_ptr<Mast>> masts);
    std::shared_ptr<Mast> RequestMast(Sailor * sailor);
    void ReleaseMast(std::shared_ptr<Mast>, Sailor * sailor);
    int OccupiedMasts();
};


#endif //PIRATESSIMULATION_MASTDISTRIBUTOR_H
