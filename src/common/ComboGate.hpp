//  Copyright (c) 2023, 2 Lift Studios
//  All rights reserved.

#pragma once

#include "rack.hpp"
#include "Gate.hpp"
#include "ManualGate.hpp"

using namespace rack;

class ComboGate : public GateBase
{
    private:
        Gate * gate;
        ManualGate * manual;

    public:
        ComboGate(Input * input, int channel, Param * param);
        virtual ~ComboGate();

        bool isHigh() override;
        bool isLow() override;
        bool isLeading() override;
        bool isTrailing() override;
        void process() override;
        void reset() override;
};