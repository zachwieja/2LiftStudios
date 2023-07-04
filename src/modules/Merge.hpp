//  Copyright (c) 2023, 2 Lift Studios
//  All rights reserved.

#pragma once

#include "plugin.hpp"
#include "CheckmarkMenuItem.hpp"
#include "Themes.hpp" 
#include "TinyGrayGreenRedButton.hpp"
#include "Utilities.hpp"


struct Merge : ThemeModule
{
    public:
        static const int NUM_ROWS = 7;

        enum ParamId
        {
            PARAM_SORT,
            PARAMS_LEN
        };

        enum InputId {
            INPUTS_LEN = NUM_ROWS
        };

        enum OutputId {
            OUTPUT_POLY,
            OUTPUTS_LEN
        };

        enum LightId {
            LIGHTS_LEN
        };

    public:
        // these are special sentinel values for the  polyphony
        // value is changed with contextMenu and stored in json

        static const int POLYPHONY_NUMBER_IN = -1;
        static const int POLYPHONY_HIGHEST_IN = 0;
        int polyphony = NUM_ROWS;

    public:
        Merge();

        void process(const ProcessArgs &args) override;
        json_t * dataToJson() override;
        void dataFromJson(json_t * root) override;
};
