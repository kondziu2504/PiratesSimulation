//
// Created by konrad on 25.12.2021.
//

#include "ShipObjectIdGenerator.h"

int ShipObjectIdGenerator::GetUniqueId() {
    return unoccupiedId++;
}

const int ShipObjectIdGenerator::kNoObject = 0;
