//  Copyright (c) 2023, 2 Lift Studios
//  All rights reserved.

#include "plugin.hpp"
#include "Buttons.hpp"
#include "Knobs.hpp"
#include "Themes.hpp"

struct Steps : ThemeModule
{
    public:
        enum ParamId {
            PARAM_CLOCK,
            PARAM_LENGTH,
            PARAM_MODE,
            PARAM_ROOT,
            PARAM_STEP,
            PARAMS_LEN
        };

        enum InputId {
            INPUT_CLOCK,
            INPUT_MODE,
            INPUT_RESET,
            INPUT_ROOT,
            INPUTS_LEN
        };

        enum OutputId {
            OUTPUT_OUTPUT,
            OUTPUTS_LEN
        };

        enum LightId {
            LIGHTS_LEN
        };

        enum Mode {
            MODE_FIRST = 0,
            MODE_RANDOM = MODE_FIRST,
            MODE_INCREMENT,
            MODE_DECREMENT,
            MODE_EXCLUSIVE,
            MODE_INCLUSIVE,
            MODE_LAST = MODE_INCLUSIVE
        };

    public:
        float lows[6]  = {-10.0f, -5.0f, -1.0f, 0.0f, 0.0f,  0.0f };
        float highs[6] = { 10.0f,  5.0f,  1.0f, 1.0f, 5.0f, 10.0f };

        int step = 0;
        int range = 0;
        int direction = 1;

        bool  clock   = 0;
        float reset   = 0.0f;
        float low     = lows[range];
        float high    = highs[range];

    public : 
        Steps()
        {
            config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

            configInput(INPUT_CLOCK, "Clock");
            configButton(PARAM_CLOCK, "Manual clock");
            configInput(INPUT_RESET, "Reset");

            configSwitch(PARAM_MODE, Mode::MODE_FIRST, Mode::MODE_LAST, Mode::MODE_RANDOM, "Mode", {"Random", "Increment", "Decrement", "Exclusive", "Inclusive" });
            configInput(INPUT_MODE, "Mode");

            configParam(PARAM_ROOT, -10.0f, 10.0f, 0.0f, "Root", "V");
            configInput(INPUT_ROOT, "Root");

            configParam(PARAM_STEP, -10.0f, 10.0f, 0.0f, "Step", "V");
            configParam(PARAM_LENGTH, 2, 100, 2, "Length", " steps");

            configOutput(OUTPUT_OUTPUT, "Output");
        }

        inline int getLength()
        {
            // the length switch returns integer values directly
            return this->params[PARAM_LENGTH].getValue();
        }

        Mode getMode()
        {
            // a switch constrains us to the enumerated Mode values
            int mode = this->params[PARAM_MODE].getValue();

            // each 1V of voltage moves the mode up or down by  one

            if (this->inputs[INPUT_MODE].isConnected()) {
                mode += (int) this->inputs[INPUT_MODE].getVoltage();
            }

            // clamp to range of modes,  and cast to an actual Mode
            return static_cast<Mode>(clamp(mode, MODE_FIRST, MODE_LAST));
        }

        float getRoot()
        {
            // the param is straight up volage in range [-10, 10] V

            float root = this->params[PARAM_ROOT].getValue();

            // if CV connected,  then add the voltage to knob value

            if (this->inputs[INPUT_ROOT].isConnected()) {
                root += this->inputs[INPUT_ROOT].getVoltage();
            }

            // keep value in the configured range  for  the  module

            return clamp(root, this->low, this->high);
        }

        inline float getStep()
        {
            // step voltage can be anything in the configured range
            return clamp(this->params[PARAM_STEP].getValue(), this->low, this->high);
        }

        inline bool isTriggered(int input, int param, bool * state)
        {
            bool temp = this->inputs[input].getVoltage() > 0.0f || this->params[param].getValue() == 1;

            if (temp && ! *state)
            {
                *state = temp;
                return true;
            }

            *state = temp;
            return false;
        }

        void process(const ProcessArgs &args) override
        {
            float reset = this->inputs[INPUT_RESET].getVoltage();

            Mode  mode   = this->getMode();
            int   length = this->getLength();

            // reset takes precedence over a clock,  if they happen
            // in same sample interval then only procces the  reset

            if (this->reset <= 0.0f && reset > 0.0f) {
                if (mode == Mode::MODE_DECREMENT)
                    this->step = length - 1;
                else if (mode == Mode::MODE_RANDOM)
                    this->step = (int)(length * random::uniform());
                else { // INCREMENT, {IN,EX}CLUSIVE
                    this->step = 0;
                }
            }

            // if we detect a new clock,  then calculate next  step

            else if (isTriggered(INPUT_CLOCK, PARAM_CLOCK, &this->clock)) {

                // need to be careful when length decreases and
                // puts the current step outside of valid range

                switch (mode) {
                    case Mode::MODE_RANDOM:
                        this->step = (int)(length * random::uniform());
                        break;

                    case Mode::MODE_INCREMENT:
                        this->step += 1;
                        if (this->step >= length) this->step = 0;
                        break;

                    case Mode::MODE_DECREMENT:
                        this->step -= 1;
                        if ((this->step < 0) || (this->step > length - 1)) this->step = length - 1;
                        break;

                    case Mode::MODE_EXCLUSIVE:
                    case Mode::MODE_INCLUSIVE:
                        this->step += this->direction;

                        if (this->step < 0) {
                            this->direction = 1;
                            this->step = mode == Mode::MODE_INCLUSIVE ? 0 : 1;
                        }
                        else if (this->step > length - 1) {
                            if (direction < 0)
                                this->step = length - 1;
                            else {
                                this->direction = -1;
                                this->step = length - (mode == Mode::MODE_INCLUSIVE ? 1 : 2);
                            }
                        }

                        break;
                }
            }

            float voltage = this->getRoot() + this->step * this->getStep();
            this->outputs[OUTPUT_OUTPUT].setVoltage(clamp(voltage, this->low, this->high));
            this->reset = reset;
        }

        json_t * dataToJson() override
        {
            json_t * root = ThemeModule::dataToJson();
            json_object_set_new(root, "range", json_integer(this->range));
            return root;
        }

        void dataFromJson(json_t * root) override
        {
            ThemeModule::dataFromJson(root);
            json_t *object = json_object_get(root, "range");
            this->range = clamp(object ? json_integer_value(object) : 0, 0, sizeof(this->lows)/sizeof(float) - 1);
        }
};


struct StepsWidget : ThemeWidget<Steps> {
    public:
        StepsWidget(Steps * module) : ThemeWidget<Steps>(module, "Steps")
        {
            setModule(module);

            // skinny module.  two screws leaves room for a label
            addChild(createWidget<ScrewSilver>(Vec(0, 0)));
            addChild(createWidget<ScrewSilver>(Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

            float x = 7.622f, y = 11.5f, dy = (109.5f - y) / 7;

            addInput(createInputCentered<PJ301MPort>(mm2px(Vec(x, y)), module, Steps::InputId::INPUT_CLOCK));
            addParam(createParamCentered<TinyTrigger>(mm2px(Vec(x + 4.4f, y + 6.25f)), module, Steps::PARAM_CLOCK));
            addInput(createInputCentered<PJ301MPort>(mm2px(Vec(x, y += dy)), module, Steps::InputId::INPUT_RESET));

            // the top and bottom ports / knobs are 14mm  apart.
            // the middle ones are pairwise and squished a  bit

            addParam(createParamCentered<Trimpot>(mm2px(Vec(x, y += dy)), module, Steps::ParamId::PARAM_MODE));
            addInput(createInputCentered<PJ301MPort>(mm2px(Vec(x, y += 9.5f)), module, Steps::InputId::INPUT_MODE));

            addParam(createParamCentered<Trimpot>(mm2px(Vec(x, y += 11.5f)), module, Steps::ParamId::PARAM_ROOT));
            addInput(createInputCentered<PJ301MPort>(mm2px(Vec(x, y += 9.5f)), module, Steps::InputId::INPUT_ROOT));

            addParam(createParamCentered<Trimpot>(mm2px(Vec(x, y += 11.5f)), module, Steps::ParamId::PARAM_STEP));
            addParam(createParamCentered<SnapTrimpot>(mm2px(Vec(x, y += dy)), module, Steps::ParamId::PARAM_LENGTH));
            addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(x, y += dy)), module, Steps::OutputId::OUTPUT_OUTPUT));
        }

        void appendContextMenu(Menu * menu) override
        {
            Steps * module = dynamic_cast<Steps *>(this->module);
            menu->addChild(new MenuSeparator);

            // the number of highs must be equal to the lows,  then
            // compute count and create labels for each menu  child

            static_assert(sizeof(module->lows) == sizeof(module->highs));
            int count = sizeof(module->lows) / sizeof(float);
            std::vector<std::string> labels;

            for (int i = 0; i < count; i++) {
                labels.push_back(string::f("[%3.1f, %3.1f]", module->lows[i], module->highs[i]));
            }

            // when they choose an item set the current  high / low

            menu->addChild(createIndexSubmenuItem(
                "Range", labels,
                [=]() { return module->range; },

                [=](int range)
                { 
                    module->low   = module->lows[range];
                    module->high  = module->highs[range];
                    module->range = range;
                }));

            ThemeWidget::appendContextMenu(menu);
        }
};

Model * modelSteps = createModel<Steps, StepsWidget>("Steps");
