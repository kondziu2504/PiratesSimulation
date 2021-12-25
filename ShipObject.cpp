//
// Created by konrad on 25.12.2021.
//

#include "ShipObject.h"

ShipObject::ShipObject(ShipObjectIdGenerator *shipObjectIdGenerator) {
    id = shipObjectIdGenerator->GetUniqueId();
}

int ShipObject::GetId() const {
    return id;
}
