//  Copyright (c) 2023, 2 Lift Studios
//  All rights reserved.

#pragma once
#include "plugin.hpp"

struct LcdDisplayWidget : TransparentWidget
{

private:
        const NVGcolor displayColor = nvgRGB(0xaf, 0xd2, 0x2c);
    
        std::shared_ptr<Font> font;
        std::string fontPath;
        char displayStr[16];

    public:
        LcdDisplayWidget();
        void drawLayer(const DrawArgs & args, int layer) override;
};