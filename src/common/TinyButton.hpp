//  Copyright (c) 2022, 2 Lift Studios
//  All rights reserved.

#pragma once
#include "plugin.hpp"

struct TinyButton : app::SVGSwitch
{
    TinyButton()
    {
        momentary = true;
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/TinyButtonUp.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/TinyButtonDown.svg")));
    }
};
