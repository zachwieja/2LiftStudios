//  Copyright (c) 2022, 2 Lift Studios
//  All rights reserved.

#include "PVolt.hpp"

PVolt::PVolt()
{
    config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

    configInput(INPUT_CLOCK, "Clock");
    configOutput(OUTPUT_OUTPUT, "Output");

    for (int row = 0; row < NUM_ROWS; row++) {
        configParam(PARAM_WEIGHT + row, 0.0f, 100.0f, 0.0f, string::f("Weight %d", row + 1));
        configParam(PARAM_OFFSET + row, -10.0f, 10.0f, 0.0f, string::f("Offset %d", row + 1), "V");
    }

    configButton(PARAM_MANUAL, "Manual");
}

void PVolt::process(const ProcessArgs &args)
{
    int row = this->row;

    float clock = this->inputs[INPUT_CLOCK].getVoltage();

    // if the gate button is pressed count that as high
    bool manual = this->params[PARAM_MANUAL].getValue() == 1;

    // if clock is now high, then we select a new row #
    // based on generated random number and the weights

    if (manual || ((this->clock <= 0) && (clock > 0))) {

        float weight = 0.0f;

        for (row = 0; row < NUM_ROWS; row++) {
            weight += this->weights[row] = this->params[PARAM_WEIGHT + row].getValue();
        }

        // if no weight then there is nothing to choose

        if (weight == 0.0f)
            row = -1;

        // if there is weight then some number of knobs
        // have been turned and we pick output  voltage

        else {
            weight *= random::uniform();

            for (row = 0; row < NUM_ROWS; row++) {
                if (weight < this->weights[row]) {
                    break;
                }

                weight -= this->weights[row];
            }
        }
    }

    // and record the current clock for the  next  pass

    this->clock = clock;

    // if row changes,  then turn off light of previous row
    // and then turn on light of the current  row,  if  any

    if (this->row != row) {
        if (this->row != -1) this->lights[this->row].value = false;
        if ((this->row = row) != -1) this->lights[this->row].value = true;
    }

    // and finally,  set the voltage based on the parameter
    // for the current row,  if any,  otherwise set to 0.0f

    this->outputs[OUTPUT_OUTPUT].setVoltage(
        this->row == -1 ? 0.0f : this->params[PARAM_OFFSET + this->row].getValue()
    );
}

json_t * PVolt::dataToJson() 
{
    json_t * root = json_object();
//    json_object_set_new(root, "polyphony", json_integer(this->polyphony));
    return root;
}

void PVolt::dataFromJson(json_t * root)
{
//    json_t * object = json_object_get(root, "polyphony");
//    this->polyphony = object ? json_integer_value(object) : MAX_CHANNELS;
}

PVoltWidget::PVoltWidget(PVolt * module)
{
    setModule(module);
    setPanel(createPanel(asset::plugin(pluginInstance, "res/PVolt.svg")));

    // 2 screws on top.  1 on bottom leaves room for  label
    addChild(createWidget<ScrewSilver>(Vec(0, 0)));
    addChild(createWidget<ScrewSilver>(Vec(box.size.x - RACK_GRID_WIDTH, 0)));
    addChild(createWidget<ScrewSilver>(Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

    for (int row = 0; row < PVolt::NUM_ROWS; row++)
    {
        addParam(createParamCentered<Trimpot>(mm2px(Vec(8.027, 11.500 + row * 11.7857142)), module, PVolt::PARAM_WEIGHT + row));
        addParam(createParamCentered<Trimpot>(mm2px(Vec(22.460, 11.500 + row * 11.7857142)), module, PVolt::PARAM_OFFSET + row));
        addChild(createLightCentered<SmallLight<GreenLight>>(mm2px(Vec(22.460 + 4.00, 11.500 + row * 11.7857142 + 4.00)), module, row));
    }

    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.027, 109.500)), module, PVolt::INPUT_CLOCK));
    addParam(createParamCentered<TinyTrigger>(mm2px(Vec(12.284, 115.168)), module, PVolt::PARAM_MANUAL));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.460, 109.500)), module, PVolt::OUTPUT_OUTPUT));
}

Model * modelPVolt = createModel<PVolt, PVoltWidget>("PVolt");
