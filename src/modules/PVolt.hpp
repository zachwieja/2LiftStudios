//  Copyright (c) 2022, 2 Lift Studios
//  All rights reserved.

#pragma once

#include "plugin.hpp"
#include "CheckmarkMenuItem.hpp"

struct PVolt : Module
{
    public:
        static const int NUM_ROWS = 8;

        enum ParamId {
            PARAM_WEIGHT,
            PARAM_OFFSET,
            PARAMS_LEN
        };

        enum InputId {
            INPUT_CLOCK,
            INPUTS_LEN
        };

        enum OutputId {
            OUTPUT_OUTPUT,
            OUTPUTS_LEN
        };

        enum LightId {
            LIGHT_OFFSET,
            LIGHTS_LEN = NUM_ROWS
        };

    public:
        int   row = -1;
        float clock = 0.0f;
        float weights[NUM_ROWS];

    public:
        PVolt();

        void process(const ProcessArgs &args) override;
        json_t * dataToJson() override;
        void dataFromJson(json_t * root) override;
};

struct PVoltWidget : ModuleWidget {
    public:
	    PVoltWidget(PVolt * module);
        void appendContextMenu(Menu * menu) override;
};
