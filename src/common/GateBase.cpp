//  Copyright (c) 2023, 2 Lift Studios
//  All rights reserved.

#include "GateBase.hpp"

void GateBase::reset() {
    this->previous = this->current = false;
}

bool GateBase::isHigh() {
    return current;
}

bool GateBase::isLow() {
    return ! current;
}

bool GateBase::isLeading() {
    return current && ! previous;
}

bool GateBase::isTrailing() {
    return previous && ! current;
}

bool GateBase::isEdge() {
    return previous != current;
}