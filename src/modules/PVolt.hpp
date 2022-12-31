//  Copyright (c) 2022, 2 Lift Studios
//  All rights reserved.

#pragma once

#include "plugin.hpp"
#include "CheckmarkMenuItem.hpp"
#include "TinyTrigger.hpp"

struct PVolt : Module
{
    public:
        static const int NUM_ROWS = 8;

        enum ParamId {
            PARAM_WEIGHT,
            PARAM_OFFSET = PARAM_WEIGHT + NUM_ROWS,
            PARAM_MANUAL = PARAM_OFFSET + NUM_ROWS,
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
};
