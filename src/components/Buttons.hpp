//  Copyright (c) 2022, 2 Lift Studios
//  All rights reserved.

#pragma once
#include "plugin.hpp"

struct TinyGrayGreenButton : app::SVGSwitch
{
    TinyGrayGreenButton()
    {
        momentary = false;
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/common/TinyButtonGray.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/common/TinyButtonGreen.svg")));
    }
};

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

struct TinyGrayRedButton : app::SVGSwitch
{
    TinyGrayRedButton()
    {
        momentary = false;
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/common/TinyButtonGray.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/common/TinyButtonRed.svg")));
    }
};

#pragma once
#include "plugin.hpp"

struct TinyTrigger : app::SVGSwitch
{
    TinyTrigger()
    {
        momentary = true;
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/common/TinyButtonGray.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/common/TinyButtonDarkGray.svg")));
    }
};
