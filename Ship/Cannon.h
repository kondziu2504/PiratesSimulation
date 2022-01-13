//
// Created by konrad on 18.05.2021.
//

#ifndef PIRATESSIMULATION_CANNON_H
#define PIRATESSIMULATION_CANNON_H

#include <memory>
#include <mutex>
#include <condition_variable>
#include "../WorldObject.h"
#include "ShipObject.h"
#include <functional>

class Sailor;
class World;
template<typename T>
using s_ptr = std::shared_ptr<T>;

class Cannon : public ShipObject {
    std::mutex ownership_mutex;
    std::pair<Sailor *, Sailor *> owners;

    WorldObject * parent;

    Vec2f local_pos;
    std::mutex loaded_mutex;
    bool loaded = false;
    std::condition_variable c_var_loaded;
public:
    explicit Cannon(Vec2f local_pos, WorldObject * parent);
    std::pair<Sailor *, Sailor *> GetOwners();
    bool Loaded();
    void WaitUntilLoadedOrTimeout();
    void Load();
    void Shoot(Vec2f target);
    bool TryClaim(Sailor * sailor);
    void Release(Sailor * sailor);
};


#endif //PIRATESSIMULATION_CANNON_H
