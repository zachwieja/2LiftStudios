//  Copyright (c) 2023, 2 Lift Studios
//  All rights reserved.

#include "plugin.hpp"
#include "Knobs.hpp"
#include "Scale.hpp"
#include "Themes.hpp"

struct Quant : ThemeModule
{
    public:
        enum ParamId {
            PARAM_SCALE,
            PARAM_ROOT,
            PARAM_OCTAVE,
            PARAMS_LEN
        };

        enum InputId {
            INPUT_SCALE,
            INPUT_ROOT,
            INPUT_OCTAVE,
            INPUT_PITCH,
            INPUTS_LEN
        };

        enum OutputId {
            OUTPUT_PITCH,
            OUTPUTS_LEN
        };

        enum LightId {
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
        Quant()
        {
            // easiest is to just hard code default scales. we allocate
            // the memory and iterate to initialize each scale in place

            this->numDefaults = 10;
            this->defaults = (Scale *) calloc(this->numDefaults, sizeof(Scale));

            if (this->defaults == NULL) {
                throw "Error allocating memory for default scales";
            }

            // the default scales are hard coded.  there is no  melodic
            // minor. this is just quantization, there is no up or down

            this->defaults[0].initialize("Major", "2212221");
            this->defaults[1].initialize("Natural Minor", "2122122");
            this->defaults[2].initialize("Harmonic Minor", "2122131");
            this->defaults[3].initialize("Pentatonic Major", "22323");
            this->defaults[4].initialize("Pentatonic Minor", "32232");
            this->defaults[5].initialize("Diminished", "21212121");
            this->defaults[6].initialize("Dominate Diminished", "12121212");
            this->defaults[7].initialize("Half Diminished", "2121222");
            this->defaults[8].initialize("Wholetone", "222222");
            this->defaults[9].initialize("Chromatic", "111111111111");

            // the default scale setting (in configSwitch above)  is  0
            // initialize module to use scale 0 from the  default  list

            Scale * scale = &this->defaults[0];
            this->steps = scale->steps;
            
            // configure the params (knobs).  some of these values  are
            // temporary.  swapScales will overwrite them (called below)

            config(ParamId::PARAMS_LEN, InputId::INPUTS_LEN, OutputId::OUTPUTS_LEN, LightId::LIGHTS_LEN);
            configSwitch(PARAM_SCALE, 0, numDefaults - 1, 0, "Scale", { "None" });
            configParam(PARAM_OCTAVE, -5, 5, 0, "Octave", "V");
            configParam(PARAM_ROOT, -scale->steps, scale->steps, 0, "Root", " steps");

            // all the inputs and outputs are pretty  straight  forward

            configInput(INPUT_SCALE, "Scale");
            configInput(INPUT_ROOT, "Root");
            configInput(INPUT_OCTAVE, "Octave");
            configInput(INPUT_PITCH, "Polyphonic");
            configOutput(OUTPUT_PITCH, "Polyphonic");

            // and when we are bypassed,  patch the input to the output.
            // just let values pass through without  any  quanitization

            configBypass(INPUT_PITCH, OUTPUT_PITCH);

            // then install the scales which will in  turn  update  the
            // labels for the scale knob for all of the default  scales

            this->swapScales(this->defaults, this->numDefaults);
        }

        ~Quant() 
        {
            if (this->scales) freeScales(&this->scales, &this->numScales);
            if (this->defaults) freeScales(&this->defaults, &this->numDefaults);
        }

        void onReset(const ResetEvent & e) override
        {
            this->swapScales(this->defaults, this->numDefaults);
            Module::onReset(e);
        }

        json_t * dataToJson() override
        {
            // you can't actually modify any of the preset values using
            // module interface.  this is just for symmetry when saving

            json_t * root = json_object();
            json_t * arr  = json_array();

            for (int i = 0; i < this->numScales; i++) 
            {
                json_t * element = json_object();
                json_object_set_new(element, "name", json_string(this->scales[i].name));
                json_object_set_new(element, "intervals", json_string(this->scales[i].intervals));
                json_array_insert_new(arr, i, element);
            }

            // and then add the scales array to the json document  root

            json_object_set_new(root, "scales", arr);

            // now write out the current knob values.  VCV writes these
            // too, but messing the knob ranges screws up the Preferences

            json_object_set_new(root, "scale", json_integer(this->params[PARAM_SCALE].getValue()));
            json_object_set_new(root, "root", json_integer(this->params[PARAM_ROOT].getValue()));

            return root;
        }

        void dataFromJson(json_t * root) override
        {
            // root contans an array of scales, find the array and then
            // iterate over it adding scales to the  vector  as  we  go

            json_t * arr = json_object_get(root, "scales");
            if (! arr) return;

            // spin array once,  to count number of scales in the  file
            // if we don't find any scales then do nothing, just return

            int numScales = 0;
            while (json_array_get(arr, numScales)) numScales++;
            if (numScales == 0) return;

            // allocate the memory,  on failure, log a warning and exit

            Scale * scales = (Scale *) calloc(numScales, sizeof(Scale));

            if (! scales) {
                WARN("Error allocating memory to read scales");
                return;
            }

            // iterate a second time,  and initialize all of the scales
            // using name and interval values within each array element

            numScales = 0;
            json_t * obj, * scale;

            while ((scale = json_array_get(arr, numScales)))
            {
                // if these elements do not exist, then we use an empty
                // string.  the scale constructor will throw  an  error

                const char * name, * intervals;

                obj = json_object_get(scale, "name");
                name = obj ? json_string_value(obj) : "";

                obj = json_object_get(scale, "intervals");
                intervals = obj ? json_string_value(obj) : "";

                try {
                    scales[numScales].initialize(name, intervals);
                }

                // if we fail to initialize any one scale then abandon
                // the whole process and undo work we have done so far

                catch (const char * message) 
                {
                    WARN("Error creating scale '%s': %s", name, message);
                    this->freeScales(&scales, &numScales);
                    return;
                }

                numScales++;
            }

            // getting here mean no errors.  swap the  newly  allocated
            // scales which also fixes up range on scale and root knobs

            this->swapScales(scales, numScales);

            // now set the knob values to saved Preferences.  vcv silently
            // catches errors. users edit the file,  give them feedback

            try {
                this->paramQuantities[PARAM_SCALE]->setDisplayValue(json_integer_value(json_object_get(root, "scale")));
                this->paramQuantities[PARAM_ROOT]->setDisplayValue(json_integer_value(json_object_get(root, "root")));
            }
            catch (const char * message) {
                WARN("Error restoring knob Preferences, %s", message);
                return;
            }
        }

        void freeScales(Scale ** scales, int * numScales)
        {
            // call destructors to give back  scales  allocated  memory

            if (scales && * scales) {
                for (int i = 0; i < * numScales; i++) {
                    (* scales)[i].~Scale();
                }

                // then free up the scales array and set things to NULL

                free(* scales);
                * scales = NULL;
                * numScales = 0;
            }
        }

        float getOctave()
        {
            // ten octave range (-5V to +5V) with the decimals cut  off

            float voltage = this->params[PARAM_OCTAVE].getValue();

            if (this->inputs[INPUT_OCTAVE].isConnected()) {
                voltage += this->params[PARAM_OCTAVE].getValue();
            }

            // the voltages are known/low. we can safely shift/truncate

            return clamp((int)(voltage + 100.0f) - 100.0f, -5.0f, 5.0f);
        }

        float getPitch(int channel)
        {
            // the pitch is a straight voltage clamped to -5V to  +5V
            // if nothing is connected then just return 0V (middle C)

            return this->inputs[INPUT_PITCH].isConnected()
                ? clamp(this->inputs[INPUT_PITCH].getVoltage(channel), -5.0f, 5.0f)
                : 0.0f;
        }

        float getRoot(Scale * scale)
        {
            // the param value is an integer step divide by  the  steps
            // for the scale to get a [0V,1V] range for the input  knob

            float root = this->params[PARAM_ROOT].getValue() / scale->steps;

            // if the CV is connected,  then add that voltage  to  knob

            if (this->inputs[INPUT_ROOT].isConnected()) {
                root += this->inputs[INPUT_ROOT].getVoltage();
            }

            // then round to closest step by multiplying  and  dividing

            float range = (scale->steps - 1.0f) / scale->steps;
            return clamp((int) (root * scale->steps) / scale->steps, -range, range);
        }

        Scale * getScale()
        {
            // we allocate 0.1V per scale (possible 100 scales at  10V)

            int index = this->inputs[INPUT_SCALE].isConnected()
                ? clamp(this->inputs[INPUT_SCALE].getVoltage(), 0.0f, this->numScales * 0.1f) / 0.1f
                : this->params[PARAM_SCALE].getValue();

            // slight adjustment here because we double the last scale

            if (index > this->numScales - 1) index = this->numScales - 1;
            Scale * scale = &this->scales[index];

            // if the number of steps in scale changes, then update the
            // root param range, and the current value within the range

            if (scale->steps != this->steps) {
                updateRootKnob(scale);
            }

            return scale;
        }

        void process(const ProcessArgs& args) override
        {
            Scale * scale = this->getScale();
            float   offset = this->getRoot(scale) + this->getOctave();
            int     channels = std::min(std::max(1, this->inputs[INPUT_PITCH].getChannels()), MAX_CHANNELS);

            // for each channel quantize to the current scale then  add
            // the offset for root and octave,  and set output  voltage

            for (int c = 0; c < channels; c++) {
                this->outputs[OUTPUT_PITCH].setVoltage(
                    scale->getClosest(this->getPitch(c)) + offset, c
                );
            }

            this->outputs[OUTPUT_PITCH].setChannels(channels);
        }

        void swapScales(Scale * scales, int numScales)
        {
            // we keep the default scales around for  module  lifetime.
            // all other scales need to be destructed and  then  freed

            if (this->scales != this->defaults) {
                this->freeScales(&this->scales, &this->numScales);
            }

            // point the module at the new scales that were  passed  in

            this->scales = scales;
            this->numScales = numScales;

            // the number of scales and scale names have likely changed
            // tell the knob to update itself with all the  new  scales

            this->updateScaleKnob();
        }

        void updateRootKnob(Scale * scale)
        {
            // get current root value to calculate the current  voltage
            // offset, and then recalculate to the new number of  steps

            float root = this->params[PARAM_ROOT].getValue() / this->steps;
            root = root * (this->steps = scale->steps);

            // give knob new range of values for root and the new value

            this->paramQuantities[PARAM_ROOT]->minValue = -(scale->steps - 1);
            this->paramQuantities[PARAM_ROOT]->maxValue = scale->steps - 1;
            this->paramQuantities[PARAM_ROOT]->setDisplayValue(root);

            // the range of the root knob is based on the steps of  the 
            // current scale, remember it so we set relative root later

            this->steps = scale->steps;
        }

        void updateScaleKnob()
        {
            // then we clear out the labels for the knob,  and build  a
            // new vector of labels,  using the current set  of  scales

            const char * name = NULL;
            SwitchQuantity * paramSwitch = (SwitchQuantity *) this->paramQuantities[PARAM_SCALE];
            paramSwitch->labels.clear();

            for (int i = 0; i < this->numScales; i++) {
                name = string::f("%d %s", i, this->scales[i].name).c_str();
                paramSwitch->labels.push_back(name);
            }

            // adjust range of motion for the knob based on # of scales

            int scaleValue = this->params[PARAM_SCALE].getValue();
            if (scaleValue > numScales) scaleValue = numScales;

            this->paramQuantities[PARAM_SCALE]->minValue = 0;
            this->paramQuantities[PARAM_SCALE]->maxValue = this->numScales - 1;
            this->paramQuantities[PARAM_SCALE]->setDisplayValue(scaleValue);
        }
};

struct QuantWidget : ThemeWidget<Quant>
{
    QuantWidget(Quant * module) : ThemeWidget<Quant>(module, "Quant")
    {
        setModule(module);

        // skinny module.  two screws leaves room for a label
        addChild(createWidget<ScrewSilver>(Vec(0, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        // column centered at 7.622mm (half of 3HP)
        float x = 7.622, y = 11.5f, dy = (109.5f - y) / 7;

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(x, y)), module, Quant::INPUT_PITCH));

        addParam(createParamCentered<SnapTrimpot>(mm2px(Vec(x, y += dy)), module, Quant::PARAM_SCALE));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(x, y += dy)), module, Quant::INPUT_SCALE));

        addParam(createParamCentered<SnapTrimpot>(mm2px(Vec(x, y += dy)), module, Quant::PARAM_ROOT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(x, y += dy)), module, Quant::INPUT_ROOT));

        addParam(createParamCentered<SnapTrimpot>(mm2px(Vec(x, y += dy)), module, Quant::PARAM_OCTAVE));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(x, y += dy)), module, Quant::INPUT_OCTAVE));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(x, y += dy)), module, Quant::OUTPUT_PITCH));
    }
};

Model * modelQuant = createModel<Quant, QuantWidget>("Quant");