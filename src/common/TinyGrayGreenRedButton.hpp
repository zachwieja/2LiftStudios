//  Copyright (c) 2022, 2 Lift Studios
//  All rights reserved.

#pragma once
#include "plugin.hpp"

struct TinyGrayGreenRedButton : app::SVGSwitch
{
    TinyGrayGreenRedButton()
    {
        momentary = false;
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/TinyButtonGray.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/TinyButtonGreen.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/TinyButtonRed.svg")));
    }
};
