//  Copyright (c) 2023, 2 Lift Studios
//  All rights reserved.

#include "plugin.hpp"
#include "Themes.hpp"
#include "Utilities.hpp"

struct Split : ThemeModule
{
    public:
        static const int NUM_ROWS = 7;
        SortOrder sortOrder;

        enum ParamId {
            PARAMS_LEN
        };

        enum InputId {
            POLY_INPUT,
            INPUTS_LEN
        };

        enum OutputId {
            OUTPUTS_LEN = NUM_ROWS
        };

        enum LightId {
            LIGHTS_LEN = NUM_ROWS
        };

    private:        
        int channels = 0;

    public:
        Split()
        {
            config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
            configInput(POLY_INPUT, "Polyphonic");

            // polyphonic can be up to 16,  but we only handle 8

            for (int c = 0; c < INPUTS_LEN; c++) {
                configOutput(c, string::f("Channel %d", c));
            }
        }

        void process(const ProcessArgs &args) override
        {
            // we only handle so many channels ignore anything more

            int channels = inputs[POLY_INPUT].getChannels();
            if (channels > NUM_ROWS) channels = NUM_ROWS;

            // we don't want different code for sorted vs unsorted. 
            // put the voltages into an array and then sort  array

            float values[channels];
            
            for (int c = 0; c < channels; c++) {
                values[c] = inputs[POLY_INPUT].getVoltage(c);
            }

            if (this->sortOrder != SortOrder::SORT_NONE) {
                Utilities::sort(values, channels, sortOrder);
            }

            // process all the incoming channels by turning on  the
            // light and passing the voltage through to the  output

            int c = 0;

            while (c < channels) {
                lights[c].value = true;
                outputs[c].setVoltage(values[c]);
                c++;
            }

            // turn off the light and clear the voltage for all the
            // outputs that just toggled their state from on to off

            while (c < this->channels) {
                lights[c].value = false;
                outputs[c].setVoltage(0.0f);
                c++;
            }

            this->channels = channels;
        }

        json_t * dataToJson() override {
            json_t * root = ThemeModule::dataToJson();
            json_object_set_new(root, "sortOrder", json_integer(this->sortOrder));
            return root;
        }

        void dataFromJson(json_t *root) override {
            ThemeModule::dataFromJson(root);
            json_t * object = json_object_get(root, "sortOrder");
            this->sortOrder = static_cast<SortOrder>(clamp(object ? json_integer_value(object) : (int) SORT_DEFAULT, SORT_MINIMUM, SORT_MAXIMUM));
        }
};

struct SplitWidget : ThemeWidget<Split, ModuleWidget>
{
    SplitWidget(Split * module) : ThemeWidget<Split>(module, "Split") 
    {
        setModule(module);

        // skinny module.  two screws leaves room for label
        addChild(createWidget<ScrewSilver>(Vec(0, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        // column centered at 7.622mm (half of 3HP)

        float x = 7.622f, y = 11.5f, dy = (109.5 - y) / module->NUM_ROWS;
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(x, y)), module, Split::POLY_INPUT));

        for (int c = 0; c < Split::NUM_ROWS; c++) {
            addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(x, y += dy)), module, c));
            addChild(createLightCentered<SmallLight<GreenLight>>(mm2px(Vec(x + 4.25f, y + 4.0f)), module, c));
        }
    }

    void appendContextMenu(Menu * menu) override
    {
        Split * module = dynamic_cast<Split *>(this->module);
        menu->addChild(new MenuSeparator);

        std::vector<std::string> labels;
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

Model *modelSplit = createModel<Split, SplitWidget>("Split");
