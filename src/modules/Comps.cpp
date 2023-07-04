//  Copyright (c) 2023, 2 Lift Studios
//  All rights reserved.

#include "plugin.hpp"
#include "CheckmarkMenuItem.hpp"
#include "Themes.hpp"
#include "TinyGrayRedButton.hpp"
#include "TinyGrayGreenRedButton.hpp"

struct Comps : ThemeModule
{
    public:
        static const int NUM_ROWS = 7;

        enum ParamId {
            PARAM_THRESH,
            PARAM_MODE = PARAM_THRESH + NUM_ROWS,
            PARAM_LOGIC = PARAM_MODE + NUM_ROWS,
            PARAMS_LEN
        };

        enum InputId {
            INPUT_INPUT,
            INPUTS_LEN
        };

        enum OutputId {
            OUTPUT_GATE,
            OUTPUT_LOGIC = NUM_ROWS,
            OUTPUTS_LEN
        };

        enum LightId {
            LIGHTS_LEN
        };

        enum Mode {
            MODE_FIRST,
            MODE_GTE = MODE_FIRST,
            MODE_LTE,
            MODE_LAST = MODE_LTE
        };

        enum Logic {
            LOGIC_FIRST,
            LOGIC_ANY = LOGIC_FIRST,
            LOGIC_ALL,
            LOGIC_NONE,
            LOGIC_LAST = LOGIC_NONE
        };

    public:
        float highs[3] = {1.0f, 5.0f, 10.0f};
        int   range    = 2;
        float high     = highs[range];

    public:
        Comps()
        {
            config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

            for (int row = 0; row < NUM_ROWS; row++) {
                configParam(PARAM_THRESH + row, -10.0f, 10.0f, 0.0f, string::f("Threshold %d", row + 1), "V");
                configOutput(OUTPUT_GATE + row, string::f("Gate %d", row + 1));
                configSwitch(PARAM_MODE + row, MODE_FIRST, MODE_LAST, MODE_GTE, string::f("Mode %d", row + 1), { "Greater Than Equal", "Less Than Equal" });
            }

            configInput(INPUT_INPUT, "Input");
            configOutput(OUTPUT_LOGIC, "Logic");
            configSwitch(PARAM_LOGIC, LOGIC_FIRST, LOGIC_LAST, LOGIC_ANY, "Logic", { "Any", "All", "None" });
        }

        inline Logic getLogic()
        {
            int value = this->params[PARAM_LOGIC].getValue();
            return static_cast<Logic>(clamp(value, LOGIC_FIRST, LOGIC_LAST));
        }

        inline Mode getMode(int row)
        {
            int value = this->params[PARAM_MODE + row].getValue();
            return static_cast<Mode>(clamp(value, MODE_FIRST, MODE_LAST));
        }

        void process(const ProcessArgs &args) override
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
                    (logic == Logic::LOGIC_ALL  && numConnected > 0 && numGates == numConnected)
                    ? this->high : 0.0f
                );
            }
        }

        json_t * dataToJson() override
        {
            json_t * root = ThemeModule::dataToJson();
            json_object_set_new(root, "range", json_integer(this->range));
            return root;
        }

        void dataFromJson(json_t *root) override
        {
            ThemeModule::dataFromJson(root);
            json_t *object = json_object_get(root, "range");
            this->range = clamp(object ? json_integer_value(object) : 0, 0, sizeof(this->highs) / sizeof(float) - 1);
        }
};

struct CompsWidget : ThemeWidget<Comps, ModuleWidget> {
    public:
        CompsWidget(Comps * module) : ThemeWidget<Comps>(module, "Comps")
        {
            setModule(module);

            // 2 screws on top.  1 on bottom leaves room for  label
            addChild(createWidget<ScrewSilver>(Vec(0, 0)));
            addChild(createWidget<ScrewSilver>(Vec(box.size.x - RACK_GRID_WIDTH, 0)));
            addChild(createWidget<ScrewSilver>(Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

            float x1 = 8.027f, x2 = 22.460, dy = (109.5f - 11.5f) / module->NUM_ROWS, y = 11.5f - dy;

            for (int row = 0; row < module->NUM_ROWS; row++) {
                addParam(createParamCentered<Trimpot>(mm2px(Vec(x1, y += dy)), module, Comps::ParamId::PARAM_THRESH + row));
                addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(x2, y)), module, Comps::OutputId::OUTPUT_GATE + row));
                addParam(createParamCentered<TinyGrayRedButton>(mm2px(Vec(x2 + 4.5f, y + 4.0f)), module, Comps::ParamId::PARAM_MODE + row));
            }

            addInput(createInputCentered<PJ301MPort>(mm2px(Vec(x1, y += dy)), module, Comps::InputId::INPUT_INPUT));
            addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(x2, y)), module, Comps::OutputId::OUTPUT_LOGIC));
            addParam(createParamCentered<TinyGrayGreenRedButton>(mm2px(Vec(x2 + 4.0f, y + 6.25f)), module, Comps::ParamId::PARAM_LOGIC));
        }

        void appendContextMenu(Menu * menu) override
        {
            Comps * module = dynamic_cast<Comps *>(this->module);
            menu->addChild(new MenuSeparator);

            std::vector<std::string> labels;

            // the number of highs must be equal to the lows,  then
            // compute count and create labels for each menu  child

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
                }
            ));

            ThemeWidget::appendContextMenu(menu);
        };
};

Model * modelComps = createModel<Comps, CompsWidget>("Comps");
