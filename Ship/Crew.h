//
// Created by konrad on 01.01.2022.
//

#ifndef PIRATESSIMULATION_CREW_H
#define PIRATESSIMULATION_CREW_H


#include <memory>
#include "Sailor.h"
#include "ShipBody.h"

class Crew : public Stopable {
    const std::shared_ptr<const std::vector<std::shared_ptr<Sailor>>> sailors;

    static std::shared_ptr<std::vector<std::shared_ptr<Sailor>>> GenerateSailors(int sailors_count, ShipBody *operated_ship, WorldObject *parent);

    void ThreadFunc(const std::atomic<bool> &stop_requested) override;

public:
    Crew(int crew_size, ShipBody * operated_ship, WorldObject * parent);
    void SetOrders(SailorOrder new_order);
    void SetCannonsTarget(WorldObject * target);
    void SetUseRightCannons(bool right);
    std::vector<std::shared_ptr<Sailor>> GetSailors();
};


#endif //PIRATESSIMULATION_CREW_H
