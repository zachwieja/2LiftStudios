//  Copyright (c) 2022, 2 Lift Studios
//  All rights reserved.

#pragma once
#include "plugin.hpp"

struct TinyGrayGreenButton : app::SVGSwitch
{
    TinyGrayGreenButton()
    {
        momentary = false;
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/TinyButtonGray.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/TinyButtonGreen.svg")));
    }
};
