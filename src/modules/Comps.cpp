//  Copyright (c) 2022, 2 Lift Studios
//  All rights reserved.

#include "Comps.hpp"

Comps::Comps()
{
    config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

    for (int row = 0; row < NUM_ROWS; row++) {
        configParam(PARAM_THRESH + row, -10.0f, 10.0f, 0.0f, string::f("Threshold %d", row + 1), "V");
        configOutput(OUTPUT_GATE + row, string::f("Gate %d", row + 1));
        configSwitch(PARAM_INVERTED + row, INVERTED_FIRST, INVERTED_LAST, INVERTED_FALSE, string::f("Inverted %d", row + 1), { "False", "True" });
    }

    configInput(INPUT_INPUT, "Input");
    configOutput(OUTPUT_LOGIC, "Logic");
    configSwitch(PARAM_LOGIC, LOGIC_FIRST, LOGIC_LAST, LOGIC_NONE, "Logic", { "None", "Any", "All" });
}

inline Comps::Logic Comps::getLogic()
{
    int value = this->params[PARAM_LOGIC].getValue();
    return static_cast<Logic>(clamp(value, LOGIC_FIRST, LOGIC_LAST));
}

inline bool Comps::getInverted(int row)
{
    int value = this->params[PARAM_INVERTED + row].getValue();
    return static_cast<Inverted>(clamp(value, INVERTED_FIRST, INVERTED_LAST)) == INVERTED_TRUE;
}

void Comps::process(const ProcessArgs &args)
{
    bool isConnected = this->inputs[INPUT_INPUT].isConnected();
    float input = this->inputs[INPUT_INPUT].getVoltage();

    // if clock is now high, then we select a new row #
    // based on generated random number and the weights

    int numConnected = 0, numGates = 0;

    for (int row = 0; row < NUM_ROWS; row++) {
        Output * output = &this->outputs[row];

        if (isConnected && output->isConnected()) {
            
            float threshold = this->params[PARAM_THRESH + row].getValue();
            bool gate = this->getInverted(row) ? input < threshold : input > threshold;

            output->setVoltage(gate ? 10.0f : 0.0f);

            numConnected += 1;
            numGates += gate ? 1 : 0;
        }
    }

    // if logic port is connected, figure out count of high
    // gates and emit one more gate based on logic  setting 

    if (this->outputs[OUTPUT_LOGIC].isConnected()) {

        Logic logic = this->getLogic();

        this->outputs[OUTPUT_LOGIC].setVoltage(
            (logic == Logic::LOGIC_NONE && numConnected > 0 && numGates == 0) ||
            (logic == Logic::LOGIC_ANY  && numGates >  0) ||
            (logic == Logic::LOGIC_ALL  && numGates == numConnected)
            ? 10.0f : 0.0f
        );
    }
}

json_t * Comps::dataToJson() 
{
    json_t * root = json_object();
//    json_object_set_new(root, "polyphony", json_integer(this->polyphony));
    return root;
}

void Comps::dataFromJson(json_t * root)
{
//    json_t * object = json_object_get(root, "polyphony");
//    this->polyphony = object ? json_integer_value(object) : MAX_CHANNELS;
}

CompsWidget::CompsWidget(Comps * module)
{
    setModule(module);
    setPanel(createPanel(asset::plugin(pluginInstance, "res/Comps.svg")));

    // 2 screws on top.  1 on bottom leaves room for  label
    addChild(createWidget<ScrewSilver>(Vec(0, 0)));
    addChild(createWidget<ScrewSilver>(Vec(box.size.x - RACK_GRID_WIDTH, 0)));
    addChild(createWidget<ScrewSilver>(Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

    for (int row = 0; row < Comps::NUM_ROWS; row++)
    {
        addParam(createParamCentered<Trimpot>(mm2px(Vec(8.027, 11.500 + row * 11.7857142)), module, Comps::ParamId::PARAM_THRESH + row));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(21.460, 11.500 + row * 11.7857142)), module, Comps::OutputId::OUTPUT_GATE + row));
        addParam(createParamCentered<TinyGrayRedButton>(mm2px(Vec(26.460, 15.500 + row * 11.7857142)), module, Comps::ParamId::PARAM_INVERTED + row));
    }

    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.027, 109.500)), module, Comps::InputId::INPUT_INPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.460, 109.500)), module, Comps::OutputId::OUTPUT_LOGIC));
    addParam(createParamCentered<TinyGrayGreenRedButton>(mm2px(Vec(26.282, 115.168)), module, Comps::ParamId::PARAM_LOGIC));
}

void CompsWidget::appendContextMenu(Menu * menu) 
{
    //Comps * module = dynamic_cast<Comps *>(this->module);
};

Model * modelComps = createModel<Comps, CompsWidget>("Comps");
