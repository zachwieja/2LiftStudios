//  Copyright (c) 2023, 2 Lift Studios
//  All rights reserved.

#pragma once

#include "rack.hpp"
using namespace rack;

class GateBase {
    protected:
        bool previous = false;
        bool current = false;

    public:
        virtual void process();
        virtual void reset();

        bool isEdge();
        bool isHigh();
        bool isLow();
        bool isLeading();
        bool isTrailing();
};