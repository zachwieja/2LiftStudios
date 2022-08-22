//  Copyright (c) 2022, 2 Lift Studios
//  All rights reserved.

#pragma once
#include "plugin.hpp"
#include "CheckmarkMenuItem.hpp"

struct Merge : Module
{
    public:
        enum ParamId
        {
            PARAMS_LEN
        };

        enum InputId
        {
            INPUTS_LEN = 8
        };

        enum OutputId
        {
            POLY_OUTPUT,
            OUTPUTS_LEN
        };

        enum LightId
        {
            LIGHTS_LEN
        };

    public:
        static const int POLYPHONY_NUMBER_IN = -1;
        static const int POLYPHONY_HIGHEST_IN = 0;
        static const int MAX_CHANNELS = INPUTS_LEN;

        bool compact = true;
        int polyphony = MAX_CHANNELS;

    public:
        Merge()
        {
            config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

            for (int c = 0; c < INPUTS_LEN; c++) {
                configInput(c, string::f("Channel %d", c));
            }

            configOutput(POLY_OUTPUT, "Polyphonic");
        }

        void process(const ProcessArgs &args) override;
        json_t * dataToJson() override;
        void dataFromJson(json_t * root) override;
};

struct MergeWidget : ModuleWidget {
    public:
	    MergeWidget(Merge * module);
        void appendContextMenu(Menu *menu) override;
};
