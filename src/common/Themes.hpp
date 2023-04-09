//  Copyright (c) 2023, 2 Lift Studios
//  All rights reserved.

#pragma once
#include "plugin.hpp"
#include "Theme.hpp"

namespace Themes {
    extern const int NUM_THEMES;

    const Theme * getTheme(int id);
    const Theme * getTheme(std::string name);
}

#include "ThemeModule.hpp"
#include "ThemeWidget.hpp"