#pragma once
#include "plugin.hpp"

#define MAX_CHANNELS 16

struct SampleAndHold : Module
{
    // there are multiple sample and holds in the  module
    // each section has section specific state saved here

    public:
        enum ParamId {
            PARAM_MODE,
            PARAMS_LEN
        };

        enum InputId
        {
            INPUT_POLY,
            INPUT_GATE,
            INPUTS_LEN
        };

        enum OutputId
        {
            OUTPUT_POLY,
            OUTPUTS_LEN
        };

        enum LightId {
            LIGHTS_LEN
        };

        enum Mode {
            MODE_FIRST = 0,
            MODE_LOW   = MODE_FIRST, // track low
            MODE_HIGH  = 1,          // track high
            MODE_SH    = 2,          // sample and hold
            MODE_LAST  = MODE_SH
        };

    public:
        struct Section
        {
            public:
                SampleAndHold::InputId gateId;
                SampleAndHold::InputId inputId;
                SampleAndHold::OutputId outputId;
                SampleAndHold::ParamId modeId;

            public:
                int gates[MAX_CHANNELS];
                float samples[MAX_CHANNELS];
        };

        Section sections[2];

    public:
        SampleAndHold() 
        {
            int numSections = sizeof(this->sections) / sizeof(Section);
            config(ParamId::PARAMS_LEN * numSections, InputId::INPUTS_LEN * numSections, OutputId::OUTPUTS_LEN * numSections, LightId::LIGHTS_LEN * numSections);

            for (int s = 0; s < numSections; s++) 
            {
                Section * section = &this->sections[s];

                section->modeId   = (ParamId) (s * PARAMS_LEN  + PARAM_MODE);
                section->gateId   = (InputId) (s * INPUTS_LEN  + INPUT_GATE);
                section->inputId  = (InputId) (s * INPUTS_LEN  + INPUT_POLY);
                section->outputId = (OutputId) (s * OUTPUTS_LEN + OUTPUT_POLY);

                for (int c = 0; c < MAX_CHANNELS; c++)
                {
                    section->gates[c] = false;
                    section->samples[c] = 0.0f;
                }

                // double the last label so each label takes a full
                // third of the  range  when  we  "spin"  the  knob

                configSwitch(section->modeId, Mode::MODE_FIRST, Mode::MODE_LAST + 1, Mode::MODE_SH, "Mode",
                    { "Track Low", "Track High", "Sample & Hold", "Sample & Hold" }
                );

                configInput(section->gateId, "Gate");
                configInput(section->inputId, "Polyphonic");
                configOutput(section->outputId, "Polyphonic");
            }
        }

        void process(const ProcessArgs &args) override;

    private:
        Mode getMode(Section * section);
};
