//  Copyright (c) 2023, 2 Lift Studios
//  All rights reserved.

#include <random>

#include "plugin.hpp"
#include "Themes.hpp"
#include "Buttons.hpp"

struct SandH : ThemeModule {

    public:
        enum ParamId {
            PARAM_MODE1,
            PARAM_MODE2,
            PARAM_GATE1,
            PARAM_GATE2,
            PARAMS_LEN
        };

        enum InputId {
            INPUT_POLY1,
            INPUT_POLY2,
            INPUT_GATE1,
            INPUT_GATE2,
            INPUTS_LEN
        };

        enum OutputId {
            OUTPUT_POLY1,
            OUTPUT_POLY2,
            OUTPUTS_LEN
        };

        enum LightId {
            LIGHTS_LEN
        };

        enum Mode {
            MODE_FIRST = -1,
            MODE_TRACK = MODE_FIRST, // pass through
            MODE_LOW = 0,            // track low
            MODE_HIGH = 1,           // track high
            MODE_SH = 2,             // sample and hold
            MODE_LAST = MODE_SH,
            MODE_DEFAULT = MODE_SH
        };

        enum Noise {
            NOISE_MINIMUM,
            NOISE_RANDOM = NOISE_MINIMUM,
            NOISE_GUASSIAN,
            NOISE_MAXIMUM = NOISE_GUASSIAN,
            NOISE_DEFAULT = NOISE_RANDOM
        };

        const float RANGE_DEFAULT = 3.0f;
        const static int MAX_CHANNELS = 16;

        std::normal_distribution<float> normal;
        std::uniform_real_distribution<float> uniform;
        std::mt19937 generator;

        Noise noise = NOISE_DEFAULT;
        float range = RANGE_DEFAULT;
        int gates[2 * MAX_CHANNELS];
        float samples[2 * MAX_CHANNELS];

    public:
        SandH() : uniform(-1.0, 1.0)
        {
            config(ParamId::PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

            for (int c = 0; c < 2 * MAX_CHANNELS; c++) {
                gates[c] = false;
                samples[c] = 0.0f;
            }

            configSwitch(PARAM_MODE1, Mode::MODE_FIRST, Mode::MODE_LAST + 1, Mode::MODE_DEFAULT, "Mode", {"Track", "Track Low", "Track High", "Sample & Hold", "Sample & Hold"});
            configInput(INPUT_GATE1, "Gate");
            configButton(PARAM_GATE1, "Manual");
            configInput(INPUT_POLY1, "Polyphonic");
            configOutput(OUTPUT_POLY1, "Polyphonic");

            configSwitch(PARAM_MODE2, Mode::MODE_FIRST, Mode::MODE_LAST + 1, Mode::MODE_DEFAULT, "Mode", {"Track", "Track Low", "Track High", "Sample & Hold", "Sample & Hold"});
            configInput(INPUT_GATE2, "Gate");
            configButton(PARAM_GATE2, "Manual");
            configInput(INPUT_POLY2, "Polyphonic");
            configOutput(OUTPUT_POLY2, "Polyphonic");
        }

        json_t * dataToJson() override
        {
            json_t * root = ThemeModule::dataToJson();
            json_object_set_new(root, "noise", json_integer(this->noise));
            json_object_set_new(root, "range", json_real(this->range));
            return root;
        }

        void dataFromJson(json_t * root) override
        {
            ThemeModule::dataFromJson(root);
            json_t * object = json_object_get(root, "noise");
            this->noise = (Noise) clamp(object ? json_integer_value(object) : (int) NOISE_DEFAULT, NOISE_MINIMUM, NOISE_MAXIMUM);

            // the stored range value is a float.  constrain it
            // to valid values of 1V,  5V,  10 or default of 3V

            object = json_object_get(root, "range");
            this->range = object ? json_real_value(object) : RANGE_DEFAULT;

            if (this->range != 1.0f && this->range != 5.0f && this->range != 10.0f) {
                this->range = RANGE_DEFAULT;
            }
        }

        inline Mode getMode(Param &param) {
            int mode = param.getValue() + 1.0f;
            if (--mode > Mode::MODE_SH) mode = Mode::MODE_SH;
            return static_cast<Mode>(mode);
        }

        inline float getNoise() {
            return (this->noise == NOISE_RANDOM ? this->uniform(this->generator) : this->normal(this->generator)) * this->range;
        }

        void processSection(Param &modeParam, Input &gateInput, Param &triggerParam, Input &inputInput, Output &outputOutput, int *gates, float *samples)
        {
            Mode mode = this->getMode(modeParam);
            bool output = outputOutput.isConnected();

            int g;
            int numGates = gateInput.getChannels();
            if (numGates > MAX_CHANNELS) numGates = MAX_CHANNELS;

            int numChannels = inputInput.getChannels();
            if (numChannels > MAX_CHANNELS) numChannels = MAX_CHANNELS;
            
            int highest = numGates > numChannels ? numGates : numChannels;
            if (highest < 1) highest = 1;

            for (int c = 0; c < highest; c++) {

                // if there aren't enough gates,  then use the last one

                if ((g = c) >= numGates) g = numGates - 1;

                // the gate is high if input is high,  or param is high

                int gate = gateInput.getVoltage(g) > 0.0f ? 1 : 0;
                gate |= triggerParam.getValue() == 1;

                // we only get samples when tracking or when  triggered
                // and we don't track when there is no output connected

                bool sample = false;

                switch (mode) {
                    case MODE_SH:    sample = gate && ! gates[c]; break;
                    case MODE_HIGH:  sample = gate && (output || ! gates[c]); break;
                    case MODE_LOW:   sample = ! gate && (output || gates[c]); break;
                    case MODE_TRACK: sample = output; break;
                }

                // get sample if needed, and store in the samples array
                // and then copy that voltage to output  (if connected)

                if (sample) samples[c] = c < numChannels ? inputInput.getVoltage(c) : getNoise();
                if (output) outputOutput.setVoltage(samples[c], c);

                // record state of gate associated with current channel

                gates[c] = gate;
            }

            outputOutput.channels = output ? highest : 0;
        }

        void process(const ProcessArgs &args) override {
            this->processSection(params[PARAM_MODE1], inputs[INPUT_GATE1], params[PARAM_GATE1], inputs[INPUT_POLY1], outputs[OUTPUT_POLY1], gates, samples);
            this->processSection(params[PARAM_MODE2], inputs[INPUT_GATE2], params[PARAM_GATE2], inputs[INPUT_POLY2], outputs[OUTPUT_POLY2], &gates[MAX_CHANNELS], &samples[MAX_CHANNELS]);
        }
};

struct SandHWidget : ThemeWidget<SandH>
{
    SandHWidget(SandH * module) : ThemeWidget<SandH>(module, "SandH")
    {
        setModule(module);

        // skinny module.  two screws leaves room for a label
        addChild(createWidget<ScrewSilver>(Vec(0, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        // it is just two sections. easiest to hard code values

        float x = 7.622, y = 11.50, dy = 13.5;
        addParam(createParamCentered<Trimpot>(mm2px(Vec(x, y)), module, SandH::PARAM_MODE1));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(x, y += dy)), module, SandH::INPUT_GATE1));
        addParam(createParamCentered<TinyTrigger>(mm2px(Vec(x + 4.0f, y + 6.25)), module, SandH::PARAM_GATE1));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(x, y += dy)), module, SandH::INPUT_POLY1));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(x, y + dy + 0.5)), module, SandH::OUTPUT_POLY1));

        y = 68.5;
        addParam(createParamCentered<Trimpot>(mm2px(Vec(x, y)), module, SandH::PARAM_MODE2));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(x, y += dy)), module, SandH::INPUT_GATE2));
        addParam(createParamCentered<TinyTrigger>(mm2px(Vec(x + 4.0f, y + 6.25)), module, SandH::PARAM_GATE2));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(x, y += dy)), module, SandH::INPUT_POLY2));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(x, y + dy + 0.5)), module, SandH::OUTPUT_POLY2));
    }

    inline float toRange(int index) {
        switch (index) {
            case 0: return 1.0f;
            case 2: return 5.0f;
            case 3: return 10.f;
            default: return 3.0f;
        }
    }

    inline int fromRange(float range) {
        if (range == 1.0f)  return 0;
        if (range == 5.0f)  return 2;
        if (range == 10.0f) return 3;
        return 1;
    }

    void appendContextMenu(Menu *menu) override
    {
        SandH * module = dynamic_cast<SandH *>(this->module);
        menu->addChild(new MenuSeparator);

        std::vector<std::string> labels;
        labels.push_back("Random");
        labels.push_back("Gaussian");

        // when they choose a noise type,  save it in the module

        menu->addChild(createIndexSubmenuItem(
            "Noise", labels,
            [=]() { return (int) module->noise; },
            [=](int noise) { module->noise = (SandH::Noise) noise; }
        ));

        labels.clear();
        labels.push_back("[-1V, 1V]");
        labels.push_back("[-3V, 3V]");
        labels.push_back("[-5V, 5V]");
        labels.push_back("[-10V, 10V]");

        // when they choose a noise type,  save it in the module

        menu->addChild(createIndexSubmenuItem(
            "Range", labels,
            [=]() { return fromRange(module->range); },
            [=](int index) { module->range = toRange(index); }
        ));

        ThemeWidget::appendContextMenu(menu);
    };
};

Model * modelSandH = createModel<SandH, SandHWidget>("SandH");
