//  Copyright (c) 2023, 2 Lift Studios
//  All rights reserved.

#include "Merge.hpp"

Merge::Merge()
{
    config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

    for (int c = 0; c < INPUTS_LEN; c++)
    {
        configInput(c, string::f("Channel %d", c));
    }

    configOutput(OUTPUT_POLY, "Polyphonic");
    configSwitch(PARAM_SORT, 0.0f, 2.0f, 0.0f, "Sort", {"None", "Ascending", "Descending"});
}

void Merge::process(const ProcessArgs &args)
{
    int channels = 0;
    float values[NUM_ROWS];

    // use configured polyphony to determine  how  channels
    // are assigned. fastest and easiest is separate  loops
 
    if (this->polyphony > 0) {

        // positive polyphony direct maps first N  channels.
        // assumes disconnected inputs  return  zero  volts

		while (channels < this->polyphony) {
            values[channels] = this->inputs[channels].getVoltage();
//			outputs[OUTPUT_POLY].setVoltage(inputs[channels].getVoltage(), channels);
            channels++;
		}
    }

    // using highest channel for polyphony,  count is equal
    // to last connected channel (zero all other  channels)
    
    else if (this->polyphony == POLYPHONY_HIGHEST_IN) {
		for (int c = 0; c < NUM_ROWS; c++) {
            if (! inputs[c].isConnected())
                values[c] = 0.0f;
                //outputs[OUTPUT_POLY].setVoltage(0.0f, c);
            else {
                values[c] = inputs[c].getVoltage();
                // outputs[OUTPUT_POLY].setVoltage(inputs[c].getVoltage(), c);
                channels = c + 1;
            }
        }
    }

    // compact channel count = number of connected channels

    else if (this->polyphony == POLYPHONY_NUMBER_IN) {
		for (int c = 0; c < NUM_ROWS; c++) {
            if (inputs[c].isConnected()) {
  				values[channels++] = this->inputs[c].getVoltage();
                // outputs[OUTPUT_POLY].setVoltage(inputs[c].getVoltage(), channels++);
            }
        }
	}

    // see if they have sorting turned on,  and do the sort

    Utilities::SortOrder sortOrder = (Utilities::SortOrder) this->params[PARAM_SORT].getValue();
    if (sortOrder != Utilities::SortOrder::SORT_NONE) Utilities::sort(values, channels, sortOrder);

    // now, copy the voltages from the array to the outputs

    for (int c = 0; c < channels; c++) {
        this->outputs[OUTPUT_POLY].setVoltage(values[c], c);
    }

    // this is the direct writing of channels (allows zero).
    // mixing with .setChannels yields inconsistent results

    this->outputs[OUTPUT_POLY].channels = channels;
}

json_t * Merge::dataToJson()
{
    json_t * root = ThemeModule::dataToJson();
    json_object_set_new(root, "polyphony", json_integer(this->polyphony));
    return root;
}

void Merge::dataFromJson(json_t * root)
{
    ThemeModule::dataFromJson(root);
    json_t * object = json_object_get(root, "polyphony");
    this->polyphony = object ? json_integer_value(object) : NUM_ROWS;
}

struct MergeWidget : ThemeWidget<Merge, ModuleWidget>
{
    MergeWidget(Merge * module) : ThemeWidget<Merge>(module, "Merge")
    {
        setModule(module);

        // skinny module.  two screws leaves room for the label
        addChild(createWidget<ScrewSilver>(Vec(0, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        // column centered at 7.622mm (half of 3HP)

        float x = 7.622f, dy = (109.5f - 11.5f) / module->NUM_ROWS, y = 11.5f - dy;
        
        for (int c = 0; c < module->NUM_ROWS; c++) {
            addInput(createInputCentered<PJ301MPort>(mm2px(Vec(x, y += dy)), module, c));
        }

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(x, y += dy)), module, Merge::OUTPUT_POLY));
        addParam(createParamCentered<TinyGrayGreenRedButton>(mm2px(Vec(x + 4.0f, y + 6.25f)), module, Merge::PARAM_SORT));
    }

    void appendContextMenu(Menu * menu) override
    {
        Merge * module = dynamic_cast<Merge *>(this->module);
        menu->addChild(new MenuSeparator);

        // this is mostly a hack, but ... dynamic and automatic
        // are special cases of polyphony with values -1 and  0

        std::vector<std::string> labels;
        labels.push_back("# Connected");
        labels.push_back("Highest #");

        for (int c = 1; c <= Merge::NUM_ROWS; c++) {
            labels.push_back(string::f("%d", c));
        }

        // the indexes are zero based. so adjust with the +/- 1.
        // > 0 is exact polyphony,  <= 0 has a special  meaning

        menu->addChild(createIndexSubmenuItem("Polyphony", labels,
            [=]() { return module->polyphony + 1;},
            [=](int polyphony) { module->polyphony = polyphony - 1;}
        ));

        ThemeWidget::appendContextMenu(menu);
    };
};

Model * modelMerge = createModel<Merge, MergeWidget>("Merge");
