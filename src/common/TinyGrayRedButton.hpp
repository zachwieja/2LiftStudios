//  Copyright (c) 2022, 2 Lift Studios
//  All rights reserved.

#pragma once
#include "plugin.hpp"

struct TinyGrayRedButton : app::SVGSwitch
{
    TinyGrayRedButton()
    {
        momentary = false;
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/common/TinyButtonGray.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/common/TinyButtonRed.svg")));
    }
};
