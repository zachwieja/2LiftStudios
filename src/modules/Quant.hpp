//  Copyright (c) 2022, 2 Lift Studios
//  All rights reserved.

#pragma once

#include "plugin.hpp"
#include "Scale.hpp"

struct Quant : Module
{
    public:
        enum ParamId
        {
            PARAM_SCALE,
            PARAM_ROOT,
            PARAM_OCTAVE,
            PARAMS_LEN
        };

        enum InputId
        {
            INPUT_SCALE,
            INPUT_ROOT,
            INPUT_OCTAVE,
            INPUT_PITCH,
            INPUTS_LEN
        };

        enum OutputId
        {
            OUTPUT_PITCH,
            OUTPUTS_LEN
        };

        enum LightId
        {
            LIGHTS_LEN
        };

    private:
        float steps = 0;
        const int MAX_CHANNELS = 16;
        
        Scale * defaults = NULL;
        int numDefaults = 0;

        Scale * scales   = NULL;
        int numScales = 0;

    public: 
        Quant();
        ~Quant();

        // these are overrides of existing vcv  module  methods

        json_t * dataToJson() override;
        void     dataFromJson(json_t * root) override;
        void     process(const ProcessArgs& args) override;
        void     onReset(const ResetEvent& e) override;

    public:

        // and these are module specific methods for processing

        void    freeScales(Scale ** scales, int * numScales);
        float   getOctave();
        float   getPitch(int channel);
        float   getRoot(Scale * scale);
        Scale * getScale();
        void    swapScales(Scale* scales, int numScales);
        void    updateRootKnob(Scale *);
        void    updateScaleKnob();
};