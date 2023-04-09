//  Copyright (c) 2023, 2 Lift Studios
//  All rights reserved.

#pragma once

#include "rack.hpp"
#include "GateBase.hpp"

using namespace rack;

class Gate : public GateBase
{
    private:
       Input * input;
       int channel;
       dsp::SchmittTrigger trigger;

    public:
        Gate(Input *input, int channel);
        virtual ~Gate();

        void process() override;
        void reset() override;
};