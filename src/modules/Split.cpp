//  Copyright (c) 2022, 2 Lift Studios
//  All rights reserved.

#include "split.hpp"

Split::Split()
{
    config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

    // there is only one input for this module

    configInput(POLY_INPUT, "Polyphonic");
    configSwitch(PARAM_SORT, 0.0f, 2.0f, 0.0f, "Sort", { "None", "Ascending", "Descending" });

    // polyphonic can be up to 16,  but we only handle 8

    for (int c = 0; c < INPUTS_LEN; c++)
    {
        configOutput(c, string::f("Channel %d", c));
    }
}

void Split::process(const ProcessArgs &args)
{
    // we only handle so many channels ignore anything more

    int channels = inputs[POLY_INPUT].getChannels();
    if (channels > MAX_CHANNELS) channels = MAX_CHANNELS;

    // we don't want different code for sorted vs unsorted. 
    // put the voltages into an array and then sort  array

    float values[channels];
    
    for (int c = 0; c < channels; c++) {
        values[c] = inputs[POLY_INPUT].getVoltage(c);
    }

    Utilities::SortOrder sortOrder = (Utilities::SortOrder) this->params[PARAM_SORT].getValue();
    if (sortOrder != Utilities::SortOrder::SORT_NONE) Utilities::sort(values, channels, sortOrder);

    // process all the incoming channels by turning on  the
    // light and passing the voltage through to the  output

    int c = 0;

    while (c < channels) {
        lights[c].value = true;
        outputs[c].setVoltage(values[c]);
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
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.622, 11.500)), module, Split::POLY_INPUT));
        addParam(createParamCentered<TinyGrayGreenRedButton>(mm2px(Vec(11.000, 17.618)), module, Split::PARAM_SORT));

        for (int c = 0; c < Split::MAX_CHANNELS; c++)
        {
            addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.622, 27.000 + c * 11.7857142)), module, c));
            addChild(createLightCentered<SmallLight<GreenLight>>(mm2px(Vec(11.872, 31.000 + c * 11.7857142)), module, c));
        }
    }
};

#if false
void SplitWidget::appendContextMenu(Menu *menu)
{
    Split * module = dynamic_cast<Split *>(this->module);
    menu->addChild(new MenuSeparator);

    // this is mostly a hack, but ... dynamic and automatic
    // are special cases of polyphony with values -1 and  0

    std::vector<std::string> labels;
    labels.push_back("# Connected");
    labels.push_back("Highest #");

    for (int c = 1; c <= Merge::MAX_CHANNELS; c++)
    {
        labels.push_back(string::f("%d", c));
    }

    // the indexes are zero based. so adjust with the +/- 1.
    // > 0 is exact polyphony,  <= 0 has a special  meaning

    menu->addChild(createIndexSubmenuItem(
        "Polyphony", labels,
        [=]()
        { return module->polyphony + 1; },
        [=](int polyphony)
        { module->polyphony = polyphony - 1; }));
};
#endif

Model *modelSplit = createModel<Split, SplitWidget>("Split");
