//
// Created by konrad on 18.05.2021.
//

#ifndef PIRATESSIMULATION_CANNON_H
#define PIRATESSIMULATION_CANNON_H

#include <memory>
#include <mutex>
#include <condition_variable>
#include "Ship.h"

class Sailor;
class World;
template<typename T>
using sp = std::shared_ptr<T>;

class Cannon : public ShipObject {
    std::mutex ownership_mutex;
    std::pair<Sailor *, Sailor *> owners;

    Ship * ship;
    float relative_pos_along;
    std::mutex loaded_mutex;
    bool loaded = false;
    std::condition_variable c_var_loaded;
public:
    explicit Cannon(Ship * ship, float relative_pos_along);
    std::pair<Sailor *, Sailor *> GetOwners();
    bool Loaded();
    void WaitUntilLoadedOrTimeout();
    void Load();
    void Shoot(Vec2 target);
    bool TryClaim(Sailor * sailor);
    void Release(Sailor * sailor);
};


#endif //PIRATESSIMULATION_CANNON_H
