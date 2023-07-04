//  Copyright (c) 2022, 2 Lift Studios
//  All rights reserved.

#pragma once
#include "plugin.hpp"

struct TinyGrayGreenRedButton : app::SVGSwitch
{
    TinyGrayGreenRedButton()
    {
        momentary = false;
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/common/TinyButtonGray.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/common/TinyButtonGreen.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/common/TinyButtonRed.svg")));
    }
};
