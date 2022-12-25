//  Copyright (c) 2022, 2 Lift Studios
//  All rights reserved.

#pragma once

#include "plugin.hpp"
#include "CheckmarkMenuItem.hpp"
#include "TinyToggle.hpp"
#include "Utilities.hpp"

struct Merge : Module
{
    public:
        enum ParamId
        {
            PARAM_SORT,
            PARAMS_LEN
        };

        enum InputId
        {
            // this is a hack. we don't have individual symbols
            // we know we have 8 inputs numbered  0  through  7

            INPUTS_LEN = 8
        };

        enum OutputId
        {
            OUTPUT_POLY,
            OUTPUTS_LEN
        };

        enum LightId
        {
            LIGHTS_LEN
        };

    public:
        // these are special sentinel values for the  polyphony

        static const int POLYPHONY_NUMBER_IN = -1;
        static const int POLYPHONY_HIGHEST_IN = 0;
        static const int MAX_CHANNELS = INPUTS_LEN;

        // value is changed with contextMenu and stored in json

        int polyphony = MAX_CHANNELS;

    public:
        Merge();

        void process(const ProcessArgs &args) override;
        json_t * dataToJson() override;
        void dataFromJson(json_t * root) override;
};

struct MergeWidget : ModuleWidget {
    public:
	    MergeWidget(Merge * module);
        void appendContextMenu(Menu *menu) override;
};
