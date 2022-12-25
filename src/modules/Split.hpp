//  Copyright (c) 2022, 2 Lift Studios
//  All rights reserved.

#pragma once

#include "plugin.hpp"
#include "TinyToggle.hpp"
#include "Utilities.hpp"

struct Split : Module
{
    public:
        enum ParamId
        {
            PARAM_SORT,
            PARAMS_LEN
        };

        enum InputId
        {
            POLY_INPUT,
            INPUTS_LEN
        };

        enum OutputId
        {
            // this is a hack. we don't have individual symbols
            // we know we have 8 outputs numbered 0  through  7

            OUTPUTS_LEN = 8
        };

        enum LightId
        {
            LIGHT_1,
            LIGHT_2,
            LIGHT_3,
            LIGHT_4,
            LIGHT_5,
            LIGHT_6,
            LIGHT_7,
            LIGHT_8,
            LIGHTS_LEN
        };

    public:
        static const int MAX_CHANNELS = 8;

    private:        
        int channels = 0;

    public:
        Split();
        void process(const ProcessArgs &args) override;
};
