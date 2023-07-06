//  Copyright (c) 2023, 2 Lift Studios
//  All rights reserved.

#include "LcdDisplayWidget.hpp"

LcdDisplayWidget::LcdDisplayWidget() {
    fontPath = std::string(asset::plugin(pluginInstance, "res/fonts/Segment14.ttf"));
}

void LcdDisplayWidget::drawLayer(const DrawArgs& args, int layer)
{
    if (layer == 1) {
        if (!(font = APP->window->loadFont(fontPath))) {
            return;
        }

        nvgFontSize(args.vg, 18);
        nvgFontFaceId(args.vg, font->handle);
        // nvgTextLetterSpacing(args.vg, 2.5);

        Vec textPos = mm2px(Vec(1.0, 1.0));
        nvgFillColor(args.vg, nvgTransRGBA(displayColor, 23));
        nvgText(args.vg, textPos.x, textPos.y, "~~~", NULL);

        nvgFillColor(args.vg, displayColor);
        snprintf(displayStr, 4, "120");

        displayStr[3] = 0; // more safety
        nvgText(args.vg, textPos.x, textPos.y, displayStr, NULL);
    }
}
