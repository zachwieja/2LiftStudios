//  Copyright (c) 2023, 2 Lift Studios
//  All rights reserved.

#include "Gate.hpp"

Gate::Gate(Input * input, int channel) {
    this->input   = input;
    this->channel = channel;
}

Gate::~Gate() {
}

void Gate::process() {
    this->previous = current;

    // when input is disconnected,  getChannels() will return 0

    if (channel >= this->input->getChannels())
        this->current = false;
    else {
        float value = this->input->getVoltage(this->channel);
        this->trigger.process(rescale(value, 0.1f, 2.0f, 0.0f, 1.0f));
        this->current = trigger.isHigh();
    }
}

void Gate::reset() {
    GateBase::reset();
    this->trigger.reset();
}
