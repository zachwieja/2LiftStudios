//  Copyright (c) 2023, 2 Lift Studios
//  All rights reserved.

#pragma once

#include "plugin.hpp"
#include "CheckmarkMenuItem.hpp"
#include "Themes.hpp"
#include "TinyGrayRedButton.hpp"
#include "TinyGrayGreenRedButton.hpp"

struct Comps : ThemeModule
{
    public:
        static const int NUM_ROWS = 7;

        enum ParamId {
            PARAM_THRESH,
            PARAM_MODE = PARAM_THRESH + NUM_ROWS,
            PARAM_LOGIC = PARAM_MODE + NUM_ROWS,
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

        enum Mode {
            MODE_FIRST,
            MODE_GTE = MODE_FIRST,
            MODE_LTE,
            MODE_LAST = MODE_LTE
        };

        enum Logic {
            LOGIC_FIRST,
            LOGIC_ANY = LOGIC_FIRST,
            LOGIC_ALL,
            LOGIC_NONE,
            LOGIC_LAST = LOGIC_NONE
        };

    public:
        float highs[3] = {1.0f, 5.0f, 10.0f};
        int   range    = 2;
        float high     = highs[range];

    public:
        Comps();
        void process(const ProcessArgs &args) override;

        json_t *dataToJson() override;
        void dataFromJson(json_t *root) override;

    private:
        Mode  getMode(int row);
        Logic getLogic();
};

struct CompsWidget : ThemeWidget<Comps, ModuleWidget> {
    public:
	    CompsWidget(Comps * module);
        void appendContextMenu(Menu * menu) override;
};
