//  Copyright (c) 2023, 2 Lift Studios
//  All rights reserved.

#pragma once

#include "plugin.hpp"
#include "CheckmarkMenuItem.hpp"
#include "Themes.hpp"
#include "TinyTrigger.hpp"

struct PVolt : ThemeModule
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
        double getWeight(int row);
        double getOffset(int row);

};

static const NVGcolor displayColOn = nvgRGB(0xaf, 0xd2, 0x2c);

struct PVoltWidget : ThemeWidget<PVolt>
{
        struct LcdDisplayWidget : TransparentWidget
        {
            PVolt * module;
            std::shared_ptr<Font> font;
            std::string fontPath;
            char displayStr[16];

            LcdDisplayWidget()
            {
                fontPath = std::string(asset::plugin(pluginInstance, "res/fonts/Segment14.ttf"));
            }

            void drawLayer(const DrawArgs &args, int layer) override
            {
                if (layer == 1) {
                    if (!(font = APP->window->loadFont(fontPath))) {
                        return;
                    }

                    nvgFontSize(args.vg, 18);
                    nvgFontFaceId(args.vg, font->handle);
                    // nvgTextLetterSpacing(args.vg, 2.5);

                    Vec textPos = mm2px(Vec(1.0, 1.0));
                    nvgFillColor(args.vg, nvgTransRGBA(displayColOn, 23));
                    nvgText(args.vg, textPos.x, textPos.y, "~~~", NULL);

                    nvgFillColor(args.vg, displayColOn);
                    snprintf(displayStr, 4, "120");

                    displayStr[3] = 0; // more safety
                    nvgText(args.vg, textPos.x, textPos.y, displayStr, NULL);
                }
            }
        };

    public:
        PVoltWidget(PVolt * module);

};
