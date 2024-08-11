//  Copyright (c) 2023, 2 Lift Studios
//  All rights reserved.

#pragma once
#include "plugin.hpp"
#include "GateBase.hpp"

class Gate : public GateBase
{
    private:
       Input * input;
       int channel;
       dsp::SchmittTrigger trigger;

    public:
        Gate(Input *input, int channel);
        virtual ~Gate();

        virtual void process() override;
        virtual void reset() override;
};