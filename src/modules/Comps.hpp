//  Copyright (c) 2022, 2 Lift Studios
//  All rights reserved.

#pragma once

#include "plugin.hpp"
#include "CheckmarkMenuItem.hpp"
#include "TinyGrayRedButton.hpp"
#include "TinyGrayGreenRedButton.hpp"

struct Comps : Module
{
    public:
        static const int NUM_ROWS = 8;

        enum ParamId
        {
            PARAM_THRESH,
            PARAM_INVERTED = PARAM_THRESH + NUM_ROWS,
            PARAM_LOGIC = PARAM_INVERTED + NUM_ROWS,
            PARAMS_LEN
        };

        enum InputId {
            INPUT_INPUT,
            INPUTS_LEN
        };

        enum OutputId {
            OUTPUT_GATE,
            OUTPUT_LOGIC = NUM_ROWS,
            OUTPUTS_LEN
        };

        enum LightId {
            LIGHTS_LEN
        };

        enum Inverted {
            INVERTED_FIRST,
            INVERTED_FALSE = INVERTED_FIRST,
            INVERTED_TRUE,
            INVERTED_LAST = INVERTED_TRUE
        };

        enum Logic {
            LOGIC_FIRST,
            LOGIC_NONE = LOGIC_FIRST,
            LOGIC_ANY,
            LOGIC_ALL,
            LOGIC_LAST = LOGIC_ALL
        };

    public:
        Comps();
        void process(const ProcessArgs &args) override;

        json_t *dataToJson() override;
        void dataFromJson(json_t *root) override;

    private:
        bool getInverted(int row);
        Logic getLogic();
};

struct CompsWidget : ModuleWidget {
    public:
	    CompsWidget(Comps * module);
        void appendContextMenu(Menu * menu) override;
};
