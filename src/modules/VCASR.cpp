//  Copyright (c) 2023, 2 Lift Studios
//  All rights reserved.

#include <float.h>
#include "plugin.hpp"

#include "Gate.hpp"
#include "ManualGate.hpp"
#include "Themes.hpp"
#include "Buttons.hpp"

struct VCASR : ThemeModule
{
    public:
        enum ParamId {
            PARAM_ATTACK,
            PARAM_MANUAL,
            PARAM_RELEASE,
            PARAM_SUSTAIN,
            PARAMS_LEN
        };

        enum InputId {
            INPUT_INPUT,
            INPUT_GATE,
            INPUTS_LEN
        };

        enum OutputId {
            OUTPUT_OUTPUT,
            OUTPUT_END,
            OUTPUT_ENV,
            OUTPUTS_LEN
        };

        enum LightId {
            LIGHTS_LEN
        };

        enum Mode {
            MODE_FIRST,
            MODE_GATED = MODE_FIRST,
            MODE_DEFAULT = MODE_GATED,
            MODE_TRIGGERED,
            MODE_LAST = MODE_TRIGGERED
        };

        enum Stage {
            STAGE_ATTACK,
            STAGE_RELEASE,
            STAGE_SUSTAIN,
            STAGE_INITIAL = STAGE_RELEASE
        };

        struct Envelope {
            Stage stage = STAGE_INITIAL;
            float value = 0.0f;
        };

        Mode mode = MODE_DEFAULT;
        ManualGate * manualGate = nullptr;

        static const int MAX_CHANNELS = 16;
        static const int MAX_GATES = MAX_CHANNELS;

        Gate * gates[MAX_CHANNELS];
        Envelope envelopes[MAX_CHANNELS];
        dsp::PulseGenerator endTriggers[MAX_CHANNELS];

    public:
        VCASR() {
            config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

            configInput(INPUT_INPUT, "Input");
            configInput(INPUT_GATE, "Gate");
            configButton(PARAM_MANUAL, "Manual Gate");

            configParam(PARAM_ATTACK,  0.0f,  60.0f,  10.0f, "Attack",  "s");
            configParam(PARAM_SUSTAIN, 0.0f, 100.0f, 100.0f, "Sustain", "%");
            configParam(PARAM_RELEASE, 0.0f,  60.0f,  10.0f, "Release", "s");

            configOutput(OUTPUT_OUTPUT, "Output");
            configOutput(OUTPUT_ENV, "Envelope");
            configOutput(OUTPUT_END, "End of cycle");

            configBypass(INPUT_INPUT, OUTPUT_OUTPUT);

            for (int c = 0; c < MAX_CHANNELS; c++) {
                this->gates[c] = new Gate(&this->inputs[INPUT_GATE], c);
            }

            this->manualGate = new ManualGate(&this->params[PARAM_MANUAL]);
        }

        ~VCASR() {
            for (int c = 0; c < MAX_CHANNELS; c++) {
                delete this->gates[c];
                this->gates[c] = nullptr;
            }

            delete this->manualGate;
            this->manualGate = nullptr;
        }

        void process(const ProcessArgs &args) override {

            // we do not want to miss leading / trailing edeges
            // process all gates,  regardless of input channels

            int numGates = this->inputs[INPUT_GATE].getChannels();
            if (numGates > MAX_GATES) numGates = MAX_GATES;

            for (int g = 0; g < numGates; g++) this->gates[g]->process();
            this->manualGate->process();

            // get values shared across all channels just  once
            // TODO - implement logarithmic attack and  release

            float sustain = this->params[PARAM_SUSTAIN].getValue() / 100.0f;
            float attack  = 1.0f / this->params[PARAM_ATTACK].getValue() * args.sampleTime;
            float release = 1.0f / this->params[PARAM_RELEASE].getValue() * args.sampleTime;

            int numChannels = this->inputs[INPUT_INPUT].getChannels();
            if (numChannels > MAX_CHANNELS) numChannels = MAX_CHANNELS;

            for (int c = 0; c < numChannels; c++) {

                this->endTriggers[c].process(args.sampleTime);

                // this first section advances  envelope  stage
                // using the polyphonic and manual gate  values

                Envelope * envelope = &this->envelopes[c];

                // choose  the highest gate <= to the channel #
                // if there is no gate use 0, which will be low

                Gate * gate = this->gates[numGates == 0 ? 0 : c < numGates ? c : numGates - 1];

                // when in triggered mode,  every  leading edge
                // toggles us between ATTACK and RELEASE stages

                if (this->mode == MODE_TRIGGERED) {
                    if (gate->isLeading() || this->manualGate->isLeading()) {
                        envelope->stage = envelope->stage == STAGE_RELEASE ? STAGE_ATTACK : STAGE_RELEASE;

                        if (envelope->stage != STAGE_ATTACK) {
                            this->endTriggers[c].trigger();
                        }
                    }
                }

                // if we are here we are not in TRIGGERED mode.
                // check the leading edge to trigger an  ATTACK

                else if (envelope->stage == STAGE_RELEASE) {
                    if ((gate->isLeading() && this->manualGate->isLow()) || (gate->isLow() && this->manualGate->isLeading())) {
                        envelope->stage = STAGE_ATTACK;
                    }
                }

                // the stage is ATTACK or SUSTAIN (not RELEASE)
                // check trailing edge to trigger  the  RELEASE

                else if ((gate->isTrailing() && this->manualGate->isLow()) || (gate->isLow() && this->manualGate->isTrailing())) {
                    envelope->stage = STAGE_RELEASE;
                }

                // if stage is ATTACK or RELEASE,  then process
                // the envelope (which might change the  stage)

                if (envelope->stage == STAGE_RELEASE) {
                    if (envelope->value > 0.0f) {
                        envelope->value -= release;

                        if (envelope->value <= 0.0f) {
                            envelope->value = 0.0f;
                            this->endTriggers[c].trigger();
                        }
                    }
                }

                else if (envelope->stage == STAGE_ATTACK) {
                    envelope->value += attack;

                    if (envelope->value >= 1.0f) {
                        envelope->value = 1.0f;
                        envelope->stage = STAGE_SUSTAIN;
                    }
                }

                // it is cheaper to set the outputs than it  is
                // to check if there are any  connected  cables

                this->outputs[OUTPUT_END].setVoltage(this->endTriggers[c].remaining > 0.0f ? 10.0f : 0.0f, c);
                this->outputs[OUTPUT_OUTPUT].setVoltage(this->inputs[INPUT_INPUT].getVoltage(c) * envelope->value * sustain, c);
                this->outputs[OUTPUT_ENV].setVoltage(envelope->value * 10.0f, c);
            }

            this->outputs[OUTPUT_OUTPUT].channels = numChannels;
            this->outputs[OUTPUT_END].channels    = numChannels;
            this->outputs[OUTPUT_ENV].channels    = numChannels;
        }

        json_t * dataToJson() override {
            json_t *root = json_object();
            json_object_set_new(root, "mode", json_integer(this->mode));
            return root;
        }

        void dataFromJson(json_t *root) override {
            json_t * object = json_object_get(root, "mode");
            this->mode     = object ? (Mode)clamp(json_integer_value(object), MODE_FIRST, MODE_LAST) : MODE_DEFAULT;
        }
};

struct VCASRWidget : ThemeWidget<VCASR>
{
    public:
        VCASRWidget(VCASR *module) : ThemeWidget<VCASR>(module, "VCASR") {
            setModule(module);

            // skinny module. two screws leaves room for the label
            addChild(createWidget<ScrewSilver>(Vec(0, 0)));
            addChild(createWidget<ScrewSilver>(Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

            // there are nine inputs/outputs/params. 8 even spaces
            // starting at 11.500 mm and then ending at 109.500 mm

            double x = 7.622, y = 11.5, dy = (109.5 - y) / 7;

            // column centered at 7.622mm (half of 3HP)
            addInput(createInputCentered<PJ301MPort>(mm2px(Vec(x, y)), module, VCASR::INPUT_INPUT));
            addInput(createInputCentered<PJ301MPort>(mm2px(Vec(x, y += dy)), module, VCASR::INPUT_GATE));
            addParam(createParamCentered<TinyTrigger>(mm2px(Vec(x + 3.5, y + 6.25)), module, VCASR::PARAM_MANUAL));

            addParam(createParamCentered<Trimpot>(mm2px(Vec(x, y += dy)), module, VCASR::PARAM_ATTACK));
            addParam(createParamCentered<Trimpot>(mm2px(Vec(x, y += dy)), module, VCASR::PARAM_SUSTAIN));
            addParam(createParamCentered<Trimpot>(mm2px(Vec(x, y += dy)), module, VCASR::PARAM_RELEASE));

            addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(x, y += dy)), module, VCASR::OUTPUT_OUTPUT));
            addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(x, y += dy)), module, VCASR::OUTPUT_END));
            addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(x, y += dy)), module, VCASR::OUTPUT_ENV));
        }

        void appendContextMenu(Menu *menu) override {
            VCASR * module = dynamic_cast<VCASR *>(this->module);
            menu->addChild(new MenuSeparator);

            std::vector<std::string> labels;
            labels.push_back("Gated");
            labels.push_back("Triggered");

            menu->addChild(createIndexSubmenuItem(
                "Mode", labels,
                [=]() { return module->mode; },
                [=](int mode) { module->mode = (VCASR::Mode) mode; }
            ));

            ThemeWidget::appendContextMenu(menu);
        };
};

Model * modelVCASR = createModel<VCASR, VCASRWidget>("VCASR");
