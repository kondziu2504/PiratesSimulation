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

class Mast;

class MastDistributor {
    std::shared_ptr<std::vector<std::shared_ptr<Mast>>> masts;
    std::shared_ptr<std::queue<std::shared_ptr<Mast>>> free_masts;

    std::mutex free_masts_mutex;
    std::condition_variable c_var_mast_freed;

public:
    explicit MastDistributor(std::shared_ptr<std::vector<std::shared_ptr<Mast>>> masts);
    void NotifySlotFreed(std::shared_ptr<Mast> mast);
    std::shared_ptr<Mast> RequestMast();
    void ReleaseMast(std::shared_ptr<Mast>);
    int OccupiedMasts();
};


#endif //PIRATESSIMULATION_MASTDISTRIBUTOR_H
