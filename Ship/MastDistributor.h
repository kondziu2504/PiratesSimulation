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
using s_ptr = std::shared_ptr<T>;

class MastDistributor {
    std::map<Mast *, std::vector<Sailor*>> masts_map_owners;
    mutable std::mutex masts_owners_mutex;
    mutable std::condition_variable c_var_mast_freed;

    Mast * FindFreeMast() const;
public:
    std::map<Mast *, std::vector<Sailor*>> GetMastsOwners() const;
    explicit MastDistributor(const std::vector<Mast *> & masts);
    Mast * RequestMast(Sailor * sailor);
    void ReleaseMast(Mast * mast, Sailor * sailor);
};


#endif //PIRATESSIMULATION_MASTDISTRIBUTOR_H
