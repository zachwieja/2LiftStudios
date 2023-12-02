//  Copyright (c) 2023, 2 Lift Studios
//  All rights reserved.

#include "plugin.hpp"

#include "Themes.hpp"
#include "Buttons.hpp"
#include "Knobs.hpp"
#include "Utilities.hpp"

struct PolyQ : ThemeModule {
    public:
        enum ParamId {
            PARAM_OCTAVE_1,
            PARAM_OCTAVE_2,
            PARAMS_LEN
        };

        enum InputId {
            INPUT_CHORD_1,
            INPUT_CHORD_2,
            INPUT_POLY_1,
            INPUT_POLY_2,
            INPUTS_LEN
        };

        enum OutputId {
            OUTPUT_POLY_1,
            OUTPUT_POLY_2,
            OUTPUTS_LEN
        };

        enum LightId {
            LIGHTS_LEN
        };

        // do not change these numbers.  down is negative,  and
        // closest and non-closest  are odd / even respectively

        enum Mode {
            MODE_DOWN = -2,
            MODE_CLOSEST_DOWN = -1,
            MODE_UP = 0,
            MODE_CLOSEST_UP = 1,

            MODE_MINIMUM = MODE_DOWN,
            MODE_MAXIMUM = MODE_CLOSEST_UP,
            MODE_DEFAULT = MODE_CLOSEST_DOWN
        };

        Mode mode = MODE_DEFAULT;
        Mode saved = mode;

    private:
        static constexpr float OCTAVE_MINIMUM = -5.0f;
        static constexpr float OCTAVE_MAXIMUM =  5.0f;
        static constexpr float OCTAVE_DEFAULT =  0.0f;

        static constexpr int   POLY_MAX = 16;
        static constexpr float EPSILON  = 0.0001;

    public:
        PolyQ()
        {
            config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

            configParam(PARAM_OCTAVE_1, OCTAVE_MINIMUM, OCTAVE_MAXIMUM, OCTAVE_DEFAULT, "Octave 1");
            configInput(INPUT_CHORD_1, "Chord 1");
            configInput(INPUT_POLY_1, "Poly 1");
            configOutput(OUTPUT_POLY_1, "Poly 1");

            configParam(PARAM_OCTAVE_2, OCTAVE_MINIMUM, OCTAVE_MAXIMUM, OCTAVE_DEFAULT, "Octave 2");
            configInput(INPUT_CHORD_2, "Chord 1");
            configInput(INPUT_POLY_2, "Poly 1");
            configOutput(OUTPUT_POLY_2, "Poly 1");

            configBypass(INPUT_POLY_1, OUTPUT_POLY_1);
            configBypass(INPUT_POLY_2, OUTPUT_POLY_2);
        }

        void process_section(Param &param, Input &chord, Input &input, Output &output)
        {
            if (! output.isConnected()) return;

            // if there's no input then set output to zero channels

            if (! input.isConnected())
                output.channels = 0;

            else {
                float octave, pitch, value, values[POLY_MAX + 2];
                int notes = chord.getChannels();
                if (notes > POLY_MAX) notes = POLY_MAX;

                // zero channels means no chord connected.  we fake
                // a chord containing a single note (quantize to C)

                if (notes == 0)
                    values[++notes] = 0.0f;

                // otherwise get the pitch for each channel and add
                // to array starting at 1 (leaving first/last open)

                else
                {
                    for (int c = 1; c <= notes; c++)
                    {
                        value = chord.getVoltage(c - 1);
                        octave = (int)(value + 100.0f) - 100;
                        pitch = value - octave;
                        values[c] = pitch < 0 ? pitch + 1 : pitch;
                    }

                    // sort the middle array  (not first and  last)
                    Utilities::sort(&values[1], notes, mode < 0 ? SORT_ASCENDING : SORT_DESCENDING);
                }

                // pad the array with the lowest and highest values
                // repeated an octave higher and  an  octave  lower

                float offset = mode < 0 ? 1.0f : -1.0f;
                values[0] = values[notes++] - offset;
                values[notes++] = values[1] + offset;

                // for each channel,  quantize the  incoming  value
                // to closest pitch in the chord and copy to output

                int channels = input.getChannels();
                if (channels > POLY_MAX) channels = POLY_MAX;

                for (int c = 0; c < channels; c++) {
                    value  = input.getVoltage(c);
                    octave = (int) (value + 100.f) - 100;
                    pitch  = value - octave;
                    if (pitch < 0) pitch += 1.0f;

                    int i = 0;

                    // if mode is odd, then it is one CLOSEST_UP or
                    // CLOSEST_DOWN,  call closest to find  closest

                    if ((this->mode & 1) == 1)
                        i = closest(pitch, values, notes);

                    // if the mode is UP or DOWN, then look for the
                    // first value greater or less than respectively

                    else if (this->mode == MODE_UP) {
                        for (i = notes - 1; i >= 0; i--) {
                            if (values[i] - pitch > EPSILON) break;
                        }
                    }
                    else if (this->mode == MODE_DOWN) {
                        for (i = notes - 1; i >= 0; i--) {
                            if (values[i] - pitch < EPSILON) break;
                        }
                    }

                    output.setVoltage(octave + values[i] + param.getValue(), c);
                }

                // cannot call setChannels(0) and  mixing  with
                // .channels breaks,  use .channels  everywhere

                output.channels = channels;
                this->saved = this->mode;
            }
        }

        void process(const ProcessArgs& args) override
        {
            process_section(this->params[PARAM_OCTAVE_1], this->inputs[INPUT_CHORD_1], this->inputs[INPUT_POLY_1], this->outputs[OUTPUT_POLY_1]);
            process_section(this->params[PARAM_OCTAVE_2], this->inputs[INPUT_CHORD_2], this->inputs[INPUT_POLY_2], this->outputs[OUTPUT_POLY_2]);
        }

        json_t *dataToJson() override
        {
            json_t *root = ThemeModule::dataToJson();
            json_object_set_new(root, "mode", json_integer(this->mode));
            return root;
        }

        void dataFromJson(json_t *root) override
        {
            ThemeModule::dataFromJson(root);
            json_t *object = json_object_get(root, "mode");
            this->mode = (Mode)clamp(object ? json_integer_value(object) : (int)MODE_DEFAULT, MODE_MINIMUM, MODE_MAXIMUM);
        }

    private:
        inline int closest(float pitch, float * values, int length) {
            if (length <= 1) return 0;

            int i = 0;
            float diff = abs(pitch - values[i]);

            // find closest pitch from ordered list. be sure to
            // account for floating point error within  EPSILON

            for (i = 1; i < length; i++) {
                float temp = abs(pitch - values[i]);
                if (abs(temp - diff) < EPSILON && values[i] != values[i - 1]) break;
                if (temp > diff) break;

                diff = temp;
            }

            return i - 1;
        }
};

struct PolyQWidget : ThemeWidget<PolyQ>
{
    PolyQWidget(PolyQ * module) : ThemeWidget<PolyQ>(module, "PolyQ")
    {
        setModule(module);

        // 2 screws on top.  1 on bottom leaves room for  label
        addChild(createWidget<ScrewSilver>(Vec(0, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        float x = 7.622, y = 11.50, dy = 13.5;
        addParam(createParamCentered<SnapTrimpot>(mm2px(Vec(x, y)),  module, PolyQ::PARAM_OCTAVE_1));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(x, y += dy)),   module, PolyQ::INPUT_CHORD_1));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(x, y += dy)),   module, PolyQ::INPUT_POLY_1));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(x, y += dy + 0.5)), module, PolyQ::OUTPUT_POLY_1));

        y = 68.5;
        addParam(createParamCentered<SnapTrimpot>(mm2px(Vec(x, y)), module, PolyQ::PARAM_OCTAVE_2));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(x, y += dy)), module, PolyQ::INPUT_CHORD_2));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(x, y += dy)), module, PolyQ::INPUT_POLY_2));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(x, y += dy + 0.5)), module, PolyQ::OUTPUT_POLY_2));
    }

    void appendContextMenu(Menu *menu) override
    {
        PolyQ * module = dynamic_cast<PolyQ *>(this->module);
        menu->addChild(new MenuSeparator);

        // values start at -2 and go to postive 1. menu will return
        // range of 0 to 3,  we subtract 2 when they choose a value

        std::vector<std::string> labels;
        labels.push_back("Down");
        labels.push_back("Closest Down");
        labels.push_back("Up");
        labels.push_back("Closest Up");

        // the indexes are zero based. so adjust with the +/- 2.
        // > 0 is exact polyphony,  <= 0 has a special  meaning

        menu->addChild(createIndexSubmenuItem(
            "Mode", labels,
            [=]() { return module->mode + 2; },
            [=](int mode) { module->mode = (PolyQ::Mode) (mode - 2); }));

        ThemeWidget::appendContextMenu(menu);
    };
};

Model * modelPolyQ = createModel<PolyQ, PolyQWidget>("PolyQ");
