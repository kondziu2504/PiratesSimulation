//
// Created by konrad on 01.01.2022.
//

#ifndef PIRATESSIMULATION_CREW_H
#define PIRATESSIMULATION_CREW_H


#include <memory>
#include "Sailor.h"
#include "ShipBody.h"

class Crew {
    const std::shared_ptr<const std::vector<std::shared_ptr<Sailor>>> sailors;

    std::shared_ptr<std::vector<std::shared_ptr<Sailor>>>
    GenerateSailors(int sailors_count, ShipBody *operated_ship, WorldObject *parent);

public:
    Crew(int crew_size, ShipBody * operated_ship, WorldObject * parent);
    void Start();
    void SetOrders(SailorOrder new_order);
    void SetCannonsTarget(WorldObject * target);
    std::vector<std::shared_ptr<Sailor>> GetSailors();
};


#endif //PIRATESSIMULATION_CREW_H
