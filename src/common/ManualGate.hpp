//  Copyright (c) 2023, 2 Lift Studios
//  All rights reserved.

#pragma once

#include "rack.hpp"
#include "GateBase.hpp"

using namespace rack;

class ManualGate : public GateBase
{
    private:
        Param * param = nullptr;

    public:
        ManualGate(Param * param);
        virtual ~ManualGate();
        void process() override;
};