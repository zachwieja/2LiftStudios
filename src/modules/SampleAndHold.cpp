#include "SampleAndHold.hpp"

SampleAndHold::Mode SampleAndHold::getMode(Section * section)
{
    int mode = params[section->modeId].getValue();
    if (mode > Mode::MODE_SH) mode = Mode::MODE_SH;
    return static_cast<Mode>(mode);
}

void SampleAndHold::process(const ProcessArgs &args)
{
    for (unsigned int s = 0; s < sizeof(this->sections) / sizeof(Section); s++)
    {
        Section * section = &this->sections[s];

        if (this->inputs[section->inputId].isConnected())
        {
            // don't copy to output if nothing is connected
            bool output = this->outputs[section->outputId].isConnected();
            Mode mode = this->getMode(section);

            int channels = std::min(this->inputs[section->inputId].getChannels(), MAX_CHANNELS);
            int gates = std::min(this->inputs[section->gateId].getChannels(), channels);

            for (int c = 0; c < channels; c++)
            {
                // if there are fewer channels on the gate, than on
                // the input, then use the last channel on the gate

                int g = std::max(std::min(c, gates - 1), 0);
                bool gate = this->inputs[section->gateId].getVoltage(g) > 0.0f ? 1 : 0;

                // if the gate button is pressed count that as high

                gate |= this->params[section->triggerId].getValue() == 1;

                // do we need to take a sample from  this  channel

                if (mode == Mode::MODE_SH) {
                    if (gate > section->gates[c]) {
                        section->samples[c] = this->inputs[section->inputId].getVoltage(c);
                    }
                }
                else if (mode == MODE_TRACK) || ((mode == Mode::MODE_LOW) ^ gate)) {
                    section->samples[c] = this->inputs[section->inputId].getVoltage(c);
                }

                // store gate value so we can detect edges for S&H

                section->gates[c] = gate;

                if (output) {
                    this->outputs[section->outputId].setVoltage(section->samples[c], c);
                }
            }

            this->outputs[section->outputId].channels = channels;
        }
        else {
            this->outputs[section->outputId].channels = 0;
        }
    }
}

struct SampleAndHoldWidget : ModuleWidget
{
    SampleAndHoldWidget(SampleAndHold * module)
    {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/SampleAndHold.svg")));

        // skinny module.  two screws leaves room for a label
        addChild(createWidget<ScrewSilver>(Vec(0, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        // it is just two sections. easiest to hard code values
        for (unsigned int s = 0; s < sizeof(module->sections) / sizeof(SampleAndHold::Section); s++) 
        {
            addParam(createParamCentered<Trimpot>(mm2px(Vec(7.622, s * 56.0 + 11.500)), module, s * SampleAndHold::PARAMS_LEN + SampleAndHold::PARAM_MODE));
            addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.622, s * 56.0 + 25.500)), module, s * SampleAndHold::INPUTS_LEN + SampleAndHold::INPUT_GATE));
            addParam(createParamCentered<TinyButton>(mm2px(Vec(11.000, s * 56.0 + 31.250)), module, s * SampleAndHold::PARAMS_LEN + SampleAndHold::PARAM_TRIGGER));
            addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.622, s * 56.0 + 39.000)), module, s * SampleAndHold::INPUTS_LEN + SampleAndHold::INPUT_POLY));
            addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.622, s * 56.0 + 52.500)), module, s * SampleAndHold::OUTPUTS_LEN + SampleAndHold::OUTPUT_POLY));
        }
    }
};

Model * modelSampleAndHold = createModel<SampleAndHold, SampleAndHoldWidget>("SampleAndHold");
