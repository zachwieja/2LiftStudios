//  Copyright (c) 2022, 2 Lift Studios
//  All rights reserved.

#include "Merge.hpp"

void Merge::process(const ProcessArgs &args)
{
    int channels = 0;

    // use configured polyphony to determine  how  channels
    // are assigned. fastest and easiest is separate  loops
 
    if (this->polyphony > 0) {

        // positive polyphony direct maps first N  channels.
        // assumes disconnected inputs  return  zero  volts

		while (channels < this->polyphony) {
			outputs[POLY_OUTPUT].setVoltage(inputs[channels].getVoltage(), channels);
            channels++;
		}
    }

    // spanning channel  count  =  last  connected  channel
    // set all disconnected channels to zero voltage
    
    else if (this->polyphony == POLYPHONY_HIGHEST_IN) {
		for (int c = 0; c < MAX_CHANNELS; c++) {
            if (! inputs[c].isConnected())
                outputs[POLY_OUTPUT].setVoltage(0.0f, c);
            else {
                outputs[POLY_OUTPUT].setVoltage(inputs[c].getVoltage(), c);
                channels = c + 1;
            }
        }
    }

    // compact channel count = number of connected channels

    else if (this->polyphony == POLYPHONY_NUMBER_IN) {
		for (int c = 0; c < MAX_CHANNELS; c++) {
            if (inputs[c].isConnected()) {
  				outputs[POLY_OUTPUT].setVoltage(inputs[c].getVoltage(), channels++);
            }
        }
	}

    // this is the direct writing of channels (allows zero).
    // mixing with .setChannels yields inconsistent results

    outputs[POLY_OUTPUT].channels = channels;
}

json_t * Merge::dataToJson() 
{
    json_t * root = json_object();
    json_object_set_new(root, "polyphony", json_integer(this->polyphony));
    return root;
}

void Merge::dataFromJson(json_t * root)
{
    json_t * object = json_object_get(root, "polyphony");
    this->polyphony = object ? json_integer_value(object) : MAX_CHANNELS;
}

MergeWidget::MergeWidget(Merge *module)
{
    setModule(module);
    setPanel(createPanel(asset::plugin(pluginInstance, "res/Merge.svg")));

    // skinny module.  two screws leaves room for the label
    addChild(createWidget<ScrewSilver>(Vec(0, 0)));
    addChild(createWidget<ScrewSilver>(Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

    // column centered at 7.622mm (half of 3HP)
    for (int c = 0; c < module->MAX_CHANNELS; c++) {
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.622, 11.875 + c * 11.732)), module, c));
    }

    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.622, 108.500)), module, Merge::POLY_OUTPUT));
}

void MergeWidget::appendContextMenu(Menu * menu) 
{
    Merge * module = dynamic_cast<Merge *>(this->module);
    menu->addChild(new MenuSeparator);

    // this is mostly a hack, but ... dynamic and automatic
    // are special cases of polyphony with values -1 and  0

    std::vector<std::string> labels;
    labels.push_back("# Connected");
    labels.push_back("Highest #");

    for (int c = 1; c <= Merge::MAX_CHANNELS; c++) {
        labels.push_back(string::f("%d", c));
    }

    // the indexes are zero based. so adjust with the +/- 1.
    // > 0 is exact polyphony,  <= 0 has a special  meaning

    menu->addChild(createIndexSubmenuItem("Polyphony", labels,
        [=]() { return module->polyphony + 1;},
        [=](int polyphony) {module->polyphony = polyphony - 1;}
    ));
};

Model * modelMerge = createModel<Merge, MergeWidget>("Merge");
