#pragma once
#include "plugin.hpp"

struct SnapTrimpot : Trimpot {
    SnapTrimpot() {
        this->snap = true;
    }
};