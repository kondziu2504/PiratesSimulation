//
// Created by konrad on 25.12.2021.
//

#ifndef PIRATESSIMULATION_OPERABLE_H
#define PIRATESSIMULATION_OPERABLE_H


#include <memory>
#include "Sailor.h"

class Operable {
public:
    virtual void Operate(Sailor * sailor) = 0;
};


#endif //PIRATESSIMULATION_OPERABLE_H
