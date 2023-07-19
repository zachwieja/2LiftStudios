//  Copyright (c) 2023, 2 Lift Studios
//  All rights reserved.

#include "plugin.hpp"

#include "Themes.hpp"
#include "Utilities.hpp"

struct Merge : ThemeModule
{
    public:
        static const int NUM_ROWS = 7;

        enum ParamId {
            PARAMS_LEN
        };

        enum InputId {
            INPUTS_LEN = NUM_ROWS
        };

        enum OutputId {
            OUTPUT_POLY,
            OUTPUTS_LEN
        };

        enum LightId {
            LIGHTS_LEN
        };

    public:
        // these are special sentinel values for the  polyphony
        // value is changed with contextMenu and stored in json

        static const int POLYPHONY_NUMBER_UNIQUE = -2;
        static const int POLYPHONY_NUMBER_IN     = -1;
        static const int POLYPHONY_HIGHEST_IN    =  0;

        int polyphony = NUM_ROWS;
        SortOrder sortOrder = SortOrder::SORT_NONE;

    public:

        Merge() 
        {
            config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

            for (int c = 0; c < INPUTS_LEN; c++) {
                configInput(c, string::f("Channel %d", c));
            }

            configOutput(OUTPUT_POLY, "Polyphonic");
        }

        void process(const ProcessArgs &args) override
        {
            int channels = 0;

            if (this->outputs[OUTPUT_POLY].isConnected()) {

                float values[NUM_ROWS];

                // use configured polyphony to determine  how  channels
                // are assigned. fastest and easiest is separate  loops
            
                if (this->polyphony > 0) {

                    // positive polyphony direct maps first N  channels.
                    // assumes disconnected inputs  return  zero  volts

                    while (channels < this->polyphony) {
                        values[channels] = this->inputs[channels].getVoltage();
                        channels++;
                    }
                }

                // using highest channel for polyphony,  count is equal
                // to last connected channel (zero all other  channels)
                
                else if (this->polyphony == POLYPHONY_HIGHEST_IN) {
                    for (int c = 0; c < NUM_ROWS; c++) {
                        if (! inputs[c].isConnected())
                            values[c] = 0.0f;
                        else {
                            values[c] = inputs[c].getVoltage();
                            channels = c + 1;
                        }
                    }
                }

                // if we get here,  then the polyphony is either unique
                // # connected,  reduce channels as dictated by setting

                else {
                    for (int c = 0; c < NUM_ROWS; c++) {
                        if (inputs[c].isConnected()) {
                            float value = this->inputs[c].getVoltage();
                            bool unique = true;

                            if (this->polyphony == POLYPHONY_NUMBER_UNIQUE) {
                                for (int i = 0; i < channels; i++) {
                                    unique &= abs(values[i] - value) > 0.001f;
                                }
                            }

                            if (unique) {
                                values[channels++] = value;
                            }
                        }
                    }
                }

                // see if they have sorting turned on, then do the sort

                if (this->sortOrder != SortOrder::SORT_NONE) {
                    Utilities::sort(values, channels, sortOrder);
                }

                // now, copy the voltages from the array to the outputs

                for (int c = 0; c < channels; c++) {
                    this->outputs[OUTPUT_POLY].setVoltage(values[c], c);
                }
            }

            // this is the direct writing of channels (allows zero).
            // mixing with .setChannels yields inconsistent results

            this->outputs[OUTPUT_POLY].channels = channels;
        }

        json_t * dataToJson() override 
        {
            json_t * root = ThemeModule::dataToJson();
            json_object_set_new(root, "polyphony", json_integer(this->polyphony));
            return root;
        }

        void dataFromJson(json_t * root) override
        {
            ThemeModule::dataFromJson(root);
            json_t * object = json_object_get(root, "polyphony");
            this->polyphony = object ? json_integer_value(object) : NUM_ROWS;
        }
};

struct MergeWidget : ThemeWidget<Merge, ModuleWidget>
{
    MergeWidget(Merge * module) : ThemeWidget<Merge>(module, "Merge")
    {
        setModule(module);

        // skinny module.  two screws leaves room for the label
        addChild(createWidget<ScrewSilver>(Vec(0, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        // column centered at 7.622mm (half of 3HP)

        float x = 7.622f, dy = (109.5f - 11.5f) / module->NUM_ROWS, y = 11.5f - dy;
        
        for (int c = 0; c < module->NUM_ROWS; c++) {
            addInput(createInputCentered<PJ301MPort>(mm2px(Vec(x, y += dy)), module, c));
        }

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(x, y += dy)), module, Merge::OUTPUT_POLY));
    }

    void appendContextMenu(Menu * menu) override
    {
        Merge * module = dynamic_cast<Merge *>(this->module);
        menu->addChild(new MenuSeparator);

        // unique, connected and highest are all special cases
        // with non-positive values -2, -1, and 0 respectively

        std::vector<std::string> labels;
        labels.push_back("# Unique");
        labels.push_back("# Connected");
        labels.push_back("Highest #");

        for (int c = 1; c <= Merge::NUM_ROWS; c++) {
            labels.push_back(string::f("%d", c));
        }

        // the indexes are zero based. so adjust with the +/- 2.
        // > 0 is exact polyphony,  <= 0 has a special  meaning

        menu->addChild(createIndexSubmenuItem("Polyphony", labels,
            [=]() { return module->polyphony + 2;},
            [=](int polyphony) { module->polyphony = polyphony - 2;}
        ));

        // next menu item control sort order of output (if any)
        
        labels.clear();
        labels.push_back("None");
        labels.push_back("Ascending");
        labels.push_back("Descending");

        menu->addChild(createIndexSubmenuItem("Sort", labels,
            [=]() { return module->sortOrder;},
            [=](int sortOrder) { module->sortOrder = (SortOrder) sortOrder; }
        ));

        ThemeWidget::appendContextMenu(menu);
    };
};

Model * modelMerge = createModel<Merge, MergeWidget>("Merge");
