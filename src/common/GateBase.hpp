//  Copyright (c) 2023, 2 Lift Studios
//  All rights reserved.

#pragma once
#include "plugin.hpp"

class GateBase {
    protected:
        bool previous = false;
        bool current = false;

    public:
        virtual ~GateBase();
        virtual void process();
        virtual void reset();

        virtual bool isEdge();
        virtual bool isHigh();
        virtual bool isLow();
        virtual bool isLeading();
        virtual bool isTrailing();
};