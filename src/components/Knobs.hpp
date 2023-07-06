//  Copyright (c) 2022, 2 Lift Studios
//  All rights reserved.

#pragma once
#include "plugin.hpp"

struct SnapTrimpot : Trimpot
{
    SnapTrimpot() {
        this->snap = true;
    }
};