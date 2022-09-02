//  Copyright (c) 2022, 2 Lift Studios
//  All rights reserved.

#include "split.hpp"

void Split::process(const ProcessArgs &args)
{
    // we only handle so many channels ignore anything more

    int channels = inputs[POLY_INPUT].getChannels();
    if (channels > MAX_CHANNELS) channels = MAX_CHANNELS;

    // process all the incoming channels by turning on  the
    // light and passing the voltage through to the  output

    int c = 0;

    while (c < channels) {
        lights[c].value = true;
        outputs[c].setVoltage(inputs[POLY_INPUT].getVoltage(c));
        c++;
    }

    // turn off the light and clear the voltage for all the
    // outputs that just toggled their state from on to off

    while (c < this->channels) {
        lights[c].value = false;
        outputs[c].setVoltage(0.0f);
        c++;
    }

    this->channels = channels;
}

struct SplitWidget : ModuleWidget
{
    SplitWidget(Split * module)
    {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/Split.svg")));

        // skinny module.  two screws leaves room for label
        addChild(createWidget<ScrewSilver>(Vec(0, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        // column centered at 7.622mm (half of 3HP)
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.622, 11.875)), module, Split::POLY_INPUT));

        for (int c = 0; c < Split::MAX_CHANNELS; c++) {
            addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.622, 26.376 + c * 11.732)), module, c));
            addChild(createLightCentered<SmallLight<GreenLight>>(mm2px(Vec(11.622, 31.000 + c * 11.732)), module, c));
        }
    }
};

Model * modelSplit = createModel<Split, SplitWidget>("Split");

