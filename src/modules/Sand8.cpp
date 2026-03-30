//  Copyright (c) 2023, 2 Lift Studios
//  All rights reserved.

#include <random>

#include "plugin.hpp"
#include "Themes.hpp"
#include "Buttons.hpp"

struct Sand8 : ThemeModule {

    public:

        const static int MAX_SANDH = 8;
        const static int MAX_CHANNELS = 16;

        enum ParamId {
            PARAM_MODE = 0,
            PARAM_GATE = PARAM_MODE + MAX_SANDH,
            PARAMS_LEN = PARAM_GATE + MAX_SANDH
        };

        enum InputId {
            INPUT_POLY = 0,
            INPUT_GATE = INPUT_POLY + MAX_SANDH,
            INPUTS_LEN = INPUT_GATE + MAX_SANDH
        };

        enum OutputId {
            OUTPUT_POLY = 0,
            OUTPUTS_LEN = OUTPUT_POLY + MAX_SANDH
        };

        enum LightId {
            LIGHTS_LEN
        };

        enum Mode {
            MODE_FIRST = -1,
            MODE_TRACK = MODE_FIRST, // pass through
            MODE_LOW = 0,            // track low
            MODE_HIGH = 1,           // track high
            MODE_SANDH = 2,          // sample and hold
            MODE_LAST = MODE_SANDH,
            MODE_DEFAULT = MODE_SANDH
        };

        enum Noise {
            NOISE_MINIMUM,
            NOISE_RANDOM = NOISE_MINIMUM,
            NOISE_GUASSIAN,
            NOISE_MAXIMUM = NOISE_GUASSIAN,
            NOISE_DEFAULT = NOISE_RANDOM
        };

        const float RANGE_DEFAULT = 3.0f;

        std::normal_distribution<float> normal;
        std::uniform_real_distribution<float> uniform;
        std::mt19937 generator;

        Noise noise = NOISE_DEFAULT;
        float range = RANGE_DEFAULT;
        int gates[MAX_SANDH * MAX_CHANNELS];
        float samples[MAX_SANDH * MAX_CHANNELS];

    public:
        Sand8() : uniform(-1.0, 1.0)
        {
            config(ParamId::PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

            for (int c = 0; c < MAX_SANDH * MAX_CHANNELS; c++) {
                gates[c] = false;
                samples[c] = 0.0f;
            }

            for (int i = 0; i < MAX_SANDH; i++) {
                configInput(INPUT_POLY   + i, "In");
                configInput(INPUT_GATE   + i, "Gate");
                configButton(PARAM_GATE  + i, "Manual");
                configSwitch(PARAM_MODE + i, Mode::MODE_FIRST, Mode::MODE_LAST, Mode::MODE_DEFAULT, "Mode", {"Track", "Track Low", "Track High", "Sample & Hold" });
                configOutput(OUTPUT_POLY + i, "Out");
            }
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
            if (--mode > Mode::MODE_SANDH) mode = Mode::MODE_SANDH;
            return static_cast<Mode>(mode);
        }

        inline float getNoise() {
            return (this->noise == NOISE_RANDOM ? this->uniform(this->generator) : this->normal(this->generator)) * this->range;
        }

        void processSandH(Param &modeParam, Input &gateInput, Param &triggerParam, Input &input, Output &output, int *gates, float *samples)
        {
            Mode mode = this->getMode(modeParam);
            bool connected = output.isConnected();

            int g;
            int numGates = gateInput.getChannels();
            if (numGates > MAX_CHANNELS) numGates = MAX_CHANNELS;

            int numChannels = input.getChannels();
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
                    case MODE_SANDH: sample = gate && ! gates[c]; break;
                    case MODE_HIGH:  sample = gate && (connected || ! gates[c]); break;
                    case MODE_LOW:   sample = ! gate && (connected || gates[c]); break;
                    case MODE_TRACK: sample = connected; break;
                }

                // get sample if needed, and store in the samples array
                // and then copy that voltage to output  (if connected)

                if (sample) samples[c] = c < numChannels ? input.getVoltage(c) : getNoise();
                if (connected) output.setVoltage(samples[c], c);

                // record state of gate associated with current channel

                gates[c] = gate;
            }

            output.channels = connected ? highest : 0;
        }

        void process(const ProcessArgs &args) override {
            for (int i = 0; i < MAX_SANDH; i++) {
                this->processSandH(
                    params[PARAM_MODE + i], inputs[INPUT_GATE + i], params[PARAM_GATE + i], inputs[INPUT_POLY + i], outputs[OUTPUT_POLY + i], 
                    &gates[MAX_CHANNELS * i], &samples[MAX_CHANNELS * i]
                );
            }                
        }
};

struct Sand8Widget : ThemeWidget<Sand8>
{
    Sand8Widget(Sand8 * module) : ThemeWidget<Sand8>(module, "Sand8")
    {
        setModule(module);

        // skinny module.  two screws leaves room for a label
        this->addChild(createWidget<ScrewSilver>(Vec(0, 0)));
        this->addChild(createWidget<ScrewSilver>(Vec(box.size.x - RACK_GRID_WIDTH, 0)));
        this->addChild(createWidget<ScrewSilver>(Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        double y = 10.75f, dy = 99.05f / (module->MAX_SANDH - 1); 
       
        for (int i = 0; i < Sand8::MAX_SANDH; i++, y += dy) {

            double x = 8.5, dx = 15.0;

            this->addInput(createInputCentered<PJ301MPort>(mm2px(Vec(x, y)), module, Sand8::INPUT_POLY + i));
            this->addInput(createInputCentered<PJ301MPort>(mm2px(Vec(x += dx, y)), module, Sand8::INPUT_GATE + i));
            this->addParam(createParamCentered<TinyTrigger>(mm2px(Vec(x + 5.0f, y + 4.5f)), module, Sand8::PARAM_GATE + i));
            this->addParam(createParamCentered<Trimpot>(mm2px(Vec(x += dx, y)), module, Sand8::PARAM_MODE + i));
            this->addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(x += dx, y)), module, Sand8::OUTPUT_POLY + i));
        }
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
        Sand8 * module = dynamic_cast<Sand8 *>(this->module);
        menu->addChild(new MenuSeparator);

        std::vector<std::string> labels;
        labels.push_back("Random");
        labels.push_back("Gaussian");

        // when they choose a noise type,  save it in the module

        menu->addChild(createIndexSubmenuItem(
            "Noise", labels,
            [=]() { return (int) module->noise; },
            [=](int noise) { module->noise = (Sand8::Noise) noise; }
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

Model * modelSand8 = createModel<Sand8, Sand8Widget>("Sand8");
