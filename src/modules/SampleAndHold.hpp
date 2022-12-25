//  Copyright (c) 2022, 2 Lift Studios
//  All rights reserved.

#pragma once
#include "plugin.hpp"
#include "TinyTrigger.hpp"

#define MAX_CHANNELS 16

struct SampleAndHold : Module
{
    // there are multiple sample and holds in the  module
    // each section has section specific state saved here

    public:
        enum ParamId {
            PARAM_MODE,
            PARAM_TRIGGER,
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
            MODE_FIRST = -1,
            MODE_TRACK = MODE_FIRST, // track (passthru)
            MODE_LOW   = 0,          // track low
            MODE_HIGH  = 1,          // track high
            MODE_SH    = 2,          // sample and hold
            MODE_LAST  = MODE_SH
        };

    public:
        struct Section
        {
            public:
                SampleAndHold::InputId  gateId;
                SampleAndHold::InputId  inputId;
                SampleAndHold::ParamId  modeId;
                SampleAndHold::OutputId outputId;
                SampleAndHold::ParamId  triggerId;

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

                section->gateId    = (InputId)  (s * INPUTS_LEN  + INPUT_GATE);
                section->inputId   = (InputId)  (s * INPUTS_LEN  + INPUT_POLY);
                section->outputId  = (OutputId) (s * OUTPUTS_LEN + OUTPUT_POLY);
                section->modeId    = (ParamId)  (s * PARAMS_LEN + PARAM_MODE);
                section->triggerId = (ParamId)  (s * PARAMS_LEN + PARAM_TRIGGER);

                for (int c = 0; c < MAX_CHANNELS; c++)
                {
                    section->gates[c] = false;
                    section->samples[c] = 0.0f;
                }

                // double the last label so each label takes a full
                // third of the  range  when  we  "spin"  the  knob

                configSwitch(section->modeId, Mode::MODE_FIRST, Mode::MODE_LAST + 1, Mode::MODE_SH, "Mode",
                    { "Track", "Track Low", "Track High", "Sample & Hold", "Sample & Hold" }
                );

                configInput(section->gateId, "Gate");
                configButton(section->triggerId, "Manual");

                configInput(section->inputId, "Polyphonic");
                configOutput(section->outputId, "Polyphonic");
            }
        }

        void process(const ProcessArgs &args) override;

    private:
        Mode getMode(Section * section);
};
