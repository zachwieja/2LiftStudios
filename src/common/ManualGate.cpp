//  Copyright (c) 2023, 2 Lift Studios
//  All rights reserved.

#include "ManualGate.hpp"

ManualGate::ManualGate(Param * param) {
    this->param = param;
}

ManualGate::~ManualGate() {
}

void ManualGate::process() {
    this->previous = current;
    this->current = this->param->getValue() > 0;
}
