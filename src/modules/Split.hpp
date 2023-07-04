//  Copyright (c) 2023, 2 Lift Studios
//  All rights reserved.

#pragma once

#include "plugin.hpp"
#include "Themes.hpp"
#include "TinyGrayGreenRedButton.hpp"
#include "Utilities.hpp"

struct Split : ThemeModule
{
    public:
        static const int NUM_ROWS = 7;

        enum ParamId {
            PARAM_SORT,
            PARAMS_LEN
        };

        enum InputId {
            POLY_INPUT,
            INPUTS_LEN
        };

        enum OutputId {
            OUTPUTS_LEN = NUM_ROWS
        };

        enum LightId {
            LIGHTS_LEN = NUM_ROWS
        };

    private:        
        int channels = 0;

    public:
        Split();
        void process(const ProcessArgs &args) override;
};
