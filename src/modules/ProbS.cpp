//  Copyright (c) 2023, 2 Lift Studios
//  All rights reserved.

#include "plugin.hpp"

#include "Themes.hpp"
#include "TinyTrigger.hpp"

struct ProbS : ThemeModule {
    public:
        static const int NUM_ROWS = 7;
        static constexpr float EPSILON = 0.001f;

        enum ParamId
        {
            PARAM_CLOCK,
            PARAM_MODE,
            PARAM_OFFSET,
            PARAM_RESET = PARAM_OFFSET + NUM_ROWS,
            PARAM_WEIGHT,
            PARAMS_LEN = PARAM_WEIGHT + NUM_ROWS
        };

        enum InputId {
            INPUT_CLOCK,
            INPUT_RESET,
            INPUTS_LEN
        };

        enum OutputId {
            OUTPUT_OUTPUT,
            OUTPUTS_LEN
        };

        enum LightId {
            LIGHTS_LEN = NUM_ROWS
        };

    private:
        enum Mode {
            MODE_FIRST,
            MODE_STOCHASTIC = MODE_FIRST,
            MODE_DEFAULT = MODE_STOCHASTIC,
            MODE_FREQUENCY,
            MODE_LAST = MODE_FREQUENCY
        };

        static const int WEIGHT_MINIMUM = 0;
        static const int WEIGHT_MAXIMUM = 100;
        static const int WEIGHT_DEFAULT = WEIGHT_MINIMUM;

        static constexpr float OFFSET_MINIMUM = -10.0f;
        static constexpr float OFFSET_MAXIMUM =  10.0f;
        static constexpr float OFFSET_DEFAULT =   0.0f;

        Mode mode = MODE_DEFAULT;
        int row   = -1;
        bool clock =  0;
        bool reset =  0;

        int weights[NUM_ROWS];
        float offsets[NUM_ROWS];

        // 800 bytes for the sample distribution
        unsigned char samples[NUM_ROWS * WEIGHT_MAXIMUM];
        int total = 0, last = -1;

    public:
        ProbS()
        {
            config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

            for (int row = 0; row < NUM_ROWS; row++) {
                this->weights[row] = 0.0f;
                configParam(PARAM_WEIGHT + row, WEIGHT_MINIMUM, WEIGHT_MAXIMUM, WEIGHT_DEFAULT, string::f("Weight %d", row + 1));
                configParam(PARAM_OFFSET + row, -10.0f, 10.0f, 0.0f, string::f("Offset %d", row + 1), "V");
            }

            configInput(INPUT_CLOCK, "Clock");
            configButton(PARAM_CLOCK, "Manual clock");
            configSwitch(PARAM_MODE, MODE_FIRST, MODE_LAST + 1, MODE_DEFAULT, "Mode", {"Stochastic", "Frequency", "Frequency"});
            configInput(INPUT_RESET, "Reset");
            configButton(PARAM_RESET, "Manual reset");
            configOutput(OUTPUT_OUTPUT, "Output");
        }

        void process(const ProcessArgs& args) override
        {
            int row = this->row;

            // if reset is triggered,  then move the index for  the
            // last sample to include the entire array  of  samples

            if (isTriggered(INPUT_RESET, PARAM_RESET, &this->reset)) {
                this->last = this->total;
            }

            // if clock is triggered, then update the samples  for
            // the weight distributions,  and then pick random row

            if (isTriggered(INPUT_CLOCK, PARAM_CLOCK, &this->clock))  {

                // if weight changes for a knob,  then fix  up  the
                // sample array to match the  updated  distribution

                for (unsigned char r = 0; r < NUM_ROWS; r++) {

                    int weight = this->getWeight(r);
                    int diff = weight - this->weights[r];

                    // if increasing the weight, add samples to end
                    // and increment the total weight (for the RNG)

                    for (int i = 0; i < diff; i++) {
                        this->samples[this->total++] = r;
                    }

                    // if decreasing the weight,  then look for and
                    // remove matching samples and fill in the gaps

                    int temp = this->total - 1;

                    while (diff < 0) {
                        if (this->samples[temp] == r) {
                            this->samples[temp] = this->samples[--this->total];
                            diff++;
                        }

                        temp--;
                    }

                    // regardless,  record current weight from knob

                    this->weights[r] = weight;
                }

                // if there is no weight,  then the offset is  zero

                if (this->total > 0) {

                    // if we are in frequency mode, then update the
                    // current range from where  we  fetch  samples

                    if ((this->getMode() == MODE_STOCHASTIC) || (--this->last < 0 || this->last >= this->total)) {
                        this->last = this->total - 1;
                    }

                    // generate random number in  range [0 .. last)

                    int index = (random::uniform() * this->last);
                    row = this->samples[index];

                    // then swap selected sample with  last  sample

                    this->samples[index] = this->samples[this->last];
                    this->samples[this->last] = row;
                }
            }

            // if row changes,  then turn off light of previous row
            // and then turn on light of the current  row,  if  any

            if (this->row != row) {
                if (this->row != -1) this->lights[this->row].value = false;
                if ((this->row = row) != -1) this->lights[this->row].value = true;
            }

            // and finally,  set the voltage based on the parameter
            // for the current row,  if any,  otherwise set to 0.0f

            this->outputs[OUTPUT_OUTPUT].setVoltage(
                this->row == -1 ? 0.0f : this->getOffset(row)
            );
        }

    private:
        inline Mode getMode() {
            int mode = this->params[PARAM_MODE].getValue();
            return static_cast<Mode>(clamp(mode, MODE_FIRST, MODE_LAST));
        }

        inline double getOffset(int row) {
            return this->params[PARAM_OFFSET + row].getValue();
        }

        inline double getWeight(int row) {
            return this->params[PARAM_WEIGHT + row].getValue();
        }

        inline bool isTriggered(int input, int param, bool * state) {
            bool temp = this->inputs[input].getVoltage() > EPSILON || this->params[param].getValue() == 1;
        
            if (temp && ! * state) {
                * state = temp;
                return true;
            }

            * state = temp;
            return false;
        }
};

struct ProbSWidget : ThemeWidget<ProbS>
{
    ProbSWidget(ProbS * module) : ThemeWidget<ProbS>(module, "ProbS")
    {
        setModule(module);

        // 2 screws on top.  1 on bottom leaves room for  label
        addChild(createWidget<ScrewSilver>(Vec(0, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        double x1 = 8.027f, x2 = 22.460, y = 11.500f, dy = 11.78571428;

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(x1, y)), module, ProbS::INPUT_CLOCK));
        addParam(createParamCentered<TinyTrigger>(mm2px(Vec(x1 + 4.4f, y + 5.75f)), module, ProbS::PARAM_CLOCK));
        this->addParam(createParamCentered<Trimpot>(mm2px(Vec(x2, y)), module, ProbS::PARAM_MODE));

        for (int row = 0; row < ProbS::NUM_ROWS; row++)
        {
                addParam(createParamCentered<Trimpot>(mm2px(Vec(x1, y += dy)), module, ProbS::PARAM_WEIGHT + row));
                addParam(createParamCentered<Trimpot>(mm2px(Vec(x2, y)), module, ProbS::PARAM_OFFSET + row));
                addChild(createLightCentered<SmallLight<GreenLight>>(mm2px(Vec(x2 + 4.000f, y + 4.000f)), module, row));
        }
        
        y = 109.5f;

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(x1, y)), module, ProbS::INPUT_RESET));
        addParam(createParamCentered<TinyTrigger>(mm2px(Vec(x1 + 4.4f, y + 5.75f)), module, ProbS::PARAM_RESET));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(x2, y)), module, ProbS::OUTPUT_OUTPUT));
    }
};

Model * modelProbS = createModel<ProbS, ProbSWidget>("ProbS");
