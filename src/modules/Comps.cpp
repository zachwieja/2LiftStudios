//  Copyright (c) 2022, 2 Lift Studios
//  All rights reserved.

#include "Comps.hpp"

Comps::Comps()
{
    config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

    for (int row = 0; row < NUM_ROWS; row++) {
        configParam(PARAM_THRESH + row, -10.0f, 10.0f, 0.0f, string::f("Threshold %d", row + 1), "V");
        configOutput(OUTPUT_GATE + row, string::f("Gate %d", row + 1));
        configSwitch(PARAM_MODE + row, MODE_FIRST, MODE_LAST, MODE_GTE, string::f("Mode %d", row + 1), { "Greater Than Equal", "Less Than Equal" });
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

inline Comps::Mode Comps::getMode(int row)
{
    int value = this->params[PARAM_MODE + row].getValue();
    return static_cast<Mode>(clamp(value, MODE_FIRST, MODE_LAST));
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
            bool gate = this->getMode(row) == MODE_LTE ? input <= threshold : input >= threshold;

            // if gate is high set output to  high  voltage

            output->setVoltage(gate ? this->high : 0.0f);

            // keep track of counts to Compsute logic  below

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
    json_t *root = json_object();
    json_object_set_new(root, "range", json_integer(this->range));
    return root;
}

void Comps::dataFromJson(json_t *root)
{
    json_t *object = json_object_get(root, "range");
    this->range = clamp(object ? json_integer_value(object) : 0, 0, sizeof(this->highs) / sizeof(float) - 1);
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
        addParam(createParamCentered<TinyGrayRedButton>(mm2px(Vec(26.460, 15.500 + row * 11.7857142)), module, Comps::ParamId::PARAM_MODE + row));
    }

    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.027, 109.500)), module, Comps::InputId::INPUT_INPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.460, 109.500)), module, Comps::OutputId::OUTPUT_LOGIC));
    addParam(createParamCentered<TinyGrayGreenRedButton>(mm2px(Vec(26.282, 115.168)), module, Comps::ParamId::PARAM_LOGIC));
}

void CompsWidget::appendContextMenu(Menu * menu) 
{
    Comps * module = dynamic_cast<Comps *>(this->module);
    menu->addChild(new MenuSeparator);

    // this is mostly a hack, but ... dynamic and automatic
    // are special cases of polyphony with values -1 and  0

    std::vector<std::string> labels;

    // the number of highs must be equal to the lows,  then
    // Compsute count and create labels for each menu  child

    int count = sizeof(module->highs) / sizeof(float);

    for (int i = 0; i < count; i++) {
        labels.push_back(string::f("[0V, %dV]", (int) module->highs[i]));
    }

    // when they choose an item set the current  high / low

    menu->addChild(createIndexSubmenuItem(
        "Gate Range", labels,
        [=]()
        { return module->range; },

        [=](int range) {
            module->high  = module->highs[range];
            module->range = range;
        }));
};

Model * modelComps = createModel<Comps, CompsWidget>("Comps");
