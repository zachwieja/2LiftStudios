//  Copyright (c) 2022, 2 Lift Studios
//  All rights reserved.

#pragma once
#include "plugin.hpp"

struct Split : Module
{
    public:
        enum ParamId
        {
            PARAMS_LEN
        };

        enum InputId
        {
            POLY_INPUT,
            INPUTS_LEN
        };

        enum OutputId
        {
            OUTPUT_1,
            OUTPUT_2,
            OUTPUT_3,
            OUTPUT_4,
            OUTPUT_5,
            OUTPUT_6,
            OUTPUT_7,
            OUTPUT_8,
            OUTPUTS_LEN
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
        dsp::ClockDivider clockDivider;

    public:
        Split()
        {
            config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

            // there is only one input for this module

            configInput(POLY_INPUT, "Polyphonic");

            // polyphonic can be up to 16,  but we only handle 8

            configOutput(OUTPUT_1, "Channel 1");
            configOutput(OUTPUT_2, "Channel 2");
            configOutput(OUTPUT_3, "Channel 3");
            configOutput(OUTPUT_4, "Channel 4");
            configOutput(OUTPUT_5, "Channel 5");
            configOutput(OUTPUT_6, "Channel 6");
            configOutput(OUTPUT_7, "Channel 7");
            configOutput(OUTPUT_8, "Channel 8");

            // used to smoothly move lights between red and green

            this->clockDivider.setDivision(16);
        }

        void process(const ProcessArgs &args) override;
};
