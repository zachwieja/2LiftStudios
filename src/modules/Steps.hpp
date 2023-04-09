//  Copyright (c) 2023, 2 Lift Studios
//  All rights reserved.

#pragma once

#include "plugin.hpp"
#include "SnapTrimpot.hpp"
#include "Themes.hpp"

struct Steps : ThemeModule
{
    public:
        enum ParamId
        {
            PARAM_LENGTH,
            PARAM_MODE,
            PARAM_ROOT,
            PARAM_STEP,
            PARAMS_LEN
        };

        enum InputId
        {
            INPUT_CLOCK,
            INPUT_MODE,
            INPUT_RESET,
            INPUT_ROOT,
            INPUTS_LEN
        };

        enum OutputId
        {
            OUTPUT_OUTPUT,
            OUTPUTS_LEN
        };

        enum LightId
        {
            LIGHTS_LEN
        };

        enum Mode
        {
            MODE_FIRST = 0,
            MODE_RANDOM = MODE_FIRST,
            MODE_INCREMENT,
            MODE_DECREMENT,
            MODE_EXCLUSIVE,
            MODE_INCLUSIVE,
            MODE_LAST = MODE_INCLUSIVE
        };

    public:
        float lows[6]  = {-10.0f, -5.0f, -1.0f, 0.0f, 0.0f,  0.0f };
        float highs[6] = { 10.0f,  5.0f,  1.0f, 1.0f, 5.0f, 10.0f };

        int step = 0;
        int range = 0;
        int direction = 1;

        float clock   =   0.0f;
        float reset   =   0.0f;
        float low     = lows[range];
        float high    = highs[range];

    public : 
        Steps();
        void process(const ProcessArgs &args) override;

        int   getLength();
        Mode  getMode();
        float getRoot();
        float getStep();
        
        json_t * dataToJson() override;
        void dataFromJson(json_t * root) override;
};

struct StepsWidget : ThemeWidget<Steps> {
    public:
	    StepsWidget(Steps * module);
        void appendContextMenu(Menu * menu) override;
};
