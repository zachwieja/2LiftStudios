//  Copyright (c) 2023, 2 Lift Studios
//  All rights reserved.

#include "plugin.hpp"
#include "Themes.hpp"

namespace Themes
{
    static const Theme themes[] = {
        Theme("Light", "res/light/", true),
        Theme("Dark", "res/dark/", false)

        // add new ones below (don't change the order)

    };

    const int NUM_THEMES = sizeof(themes) / sizeof(themes[0]);

    const Theme * getTheme(int id) {
        return &themes[clamp(id, 0, Themes::NUM_THEMES - 1)];
    }

    const Theme * getTheme(std::string name) {
        for (int i = 0; i < NUM_THEMES; i++) {
            if (themes[i].getName() == name) return &themes[i];
        }

        return &themes[0];
    }
};