//
// Created by konrad on 25.12.2021.
//

#ifndef PIRATESSIMULATION_SHIPOBJECT_H
#define PIRATESSIMULATION_SHIPOBJECT_H

#include <memory>
#include "ShipObjectIdGenerator.h"

class ShipObject {
private:
    int id;

public:
    explicit ShipObject(ShipObjectIdGenerator * shipObjectIdGenerator);
    [[nodiscard]]
    int GetId() const;
};


#endif //PIRATESSIMULATION_SHIPOBJECT_H
