//  Copyright (c) 2023, 2 Lift Studios
//  All rights reserved.

#include "ComboGate.hpp"

ComboGate::ComboGate(Input * input, int channel, Param * param) {
    this->gate = new Gate(input, channel);
    this->manual = new ManualGate(param);
}

ComboGate::~ComboGate() {
}

void ComboGate::process() {
    this->gate->process();
    this->manual->process();

    this->previous = this->current;
    this->current  = this->isHigh();
}

void ComboGate::reset() {
    this->gate->reset();
    this->manual->reset();
}

bool ComboGate::isHigh() {
    return gate->isHigh() || manual->isHigh();
}

bool ComboGate::isLow() {
    return gate->isLow() && manual->isLow();
}

bool ComboGate::isLeading() {
    return gate->isLeading() && manual->isLow() || manual->isLeading() && gate->isLow();
}

bool ComboGate::isTrailing() {
    return gate->isTrailing() && manual->isLow() || manual->isTrailing() && gate->isLow();
}
