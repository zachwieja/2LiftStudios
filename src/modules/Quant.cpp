#include "plugin.hpp"
#include "Quant.hpp"

Quant::Quant() {
    assert(this->intervals.size() == this->names.size());

    this->numScales = this->names.size();
    this->scales = (Scale**) std::calloc(this->numScales, sizeof(Scale*));

    for (int i = 0; i < numScales; i++)
    {
        this->scales[i] = new Scale(this->intervals[i]);
    }

    Scale * scale = this->scales[0];

    // save number of steps in the current scale.  then
    // if scale changes we update the root param  range

    this->steps = scale->steps;

    config(ParamId::PARAMS_LEN, InputId::INPUTS_LEN, OutputId::OUTPUTS_LEN, LightId::LIGHTS_LEN);

    configParam(PARAM_ROOT, 0.0f, scale->steps - 1, 0.0f, "Root");
    configSwitch(PARAM_SCALE, 0, this->numScales - 1, 0, "Scale", names);
    configParam(PARAM_OCTAVE, -5, 5, 0, "Octave");

    configInput(INPUT_SCALE, "Scale");
    configInput(INPUT_OCTAVE, "Octave");
    configInput(INPUT_PITCH, "Polyphonic");
    configOutput(OUTPUT_PITCH, "Polyphonic");

    configBypass(INPUT_PITCH, OUTPUT_PITCH);
}

Quant::~Quant() {
    if (this->scales != NULL) {
        for (int i = 0; i < this->numScales; i++) {
            if (this->scales[i]) {
                free(this->scales[i]);
                this->scales[i] = NULL;
            }
        }

        free(this->scales);
        this->scales = NULL;
    }
}

json_t* Quant::dataToJson()
{
    json_t * root = json_object();
    json_object_set_new(root, "relativeOctave", json_boolean(this->relativeOctave));
    json_object_set_new(root, "relativeRoot", json_boolean(this->relativeRoot));
    return root;
}

void Quant::dataFromJson(json_t* root)
{
    json_t* object;
    
    object = json_object_get(root, "relativeOctave");
    this->relativeOctave = object ? json_boolean_value(object) : true;
    object = json_object_get(root, "relativeRoot");
    this->relativeRoot = object ? json_boolean_value(object) : true;
}

float Quant::getOctave()
{
    // ten octave range (-5V to +5V) with the decimals cut  off

    float voltage;

    if (! this->inputs[INPUT_OCTAVE].isConnected())
        voltage = this->params[PARAM_OCTAVE].getValue();
    else {
        voltage = this->inputs[INPUT_OCTAVE].getVoltage();
        if (this->relativeOctave) voltage += this->params[PARAM_OCTAVE].getValue();
        voltage = clamp((int)(voltage + 100.0f) - 100.0f, -5.0f, 5.0f);
    }

    return voltage;
}

float Quant::getPitch(int channel)
{
    // the pitch is a straight voltage clamped to -5V to  +5V
    // if nothing is connected then just return 0V (middle C)

    return this->inputs[INPUT_PITCH].isConnected()
        ? clamp(this->inputs[INPUT_PITCH].getVoltage(channel), -5.0f, 5.0f)
        : 0.0f;
}

float Quant::getRoot(Scale * scale)
{
    // the param value is an integer step. the input voltage is
    // a value between [0V,1V]. Multiply to get the step number

    float root;

    if (! this->inputs[INPUT_ROOT].isConnected())
        root = this->params[PARAM_ROOT].getValue() / scale->steps;
    else {
        root = this->inputs[INPUT_ROOT].getVoltage();
        if (this->relativeRoot) root += this->params[PARAM_ROOT].getValue() / scale->steps;
        root = (int) (root * scale->steps) / scale->steps;
    }

    return clamp(root, 0.0f, 1.0f);
}

Scale * Quant::getScale()
{
    // we allocate 0.1V per scale (possible 100 scales).  order
    // of scales is maintained across versions for back  compat

    int index = this->inputs[INPUT_SCALE].isConnected()
        ? clamp(this->inputs[INPUT_SCALE].getVoltage(), 0.0f, this->numScales * 0.1f) / 0.1f
        : this->params[PARAM_SCALE].getValue();

    if (index > this->numScales - 1) index = this->numScales - 1;
    Scale * scale = this->scales[index];

    // if the number of steps in scale changes, then update the
    // root param range, and the current value within the range

    if (scale->steps != this->steps) 
    {
        float root = this->params[PARAM_ROOT].getValue() / this->steps;
        root = root * (this->steps = scale->steps);

        paramQuantities[PARAM_ROOT]->maxValue = scale->steps - 1;
        paramQuantities[PARAM_ROOT]->setDisplayValue(root);
    }

    return scale;
}

void Quant::process(const ProcessArgs &args)
{
    Scale * scale     = this->getScale();
    float   offset    = this->getRoot(scale) + this->getOctave();
    int     channels  = std::min(std::max(1, this->inputs[INPUT_PITCH].getChannels()), MAX_CHANNELS);

    // for each channel quantize to the current scale then  add
    // the offset for root and octave,  and set output  voltage

    for (int c = 0; c < channels; c++) {
        float input = this->getPitch(c);
        float pitch = scale->getClosest(input) + offset;
        this->outputs[OUTPUT_PITCH].setVoltage(pitch, c);
    }

    this->outputs[OUTPUT_PITCH].setChannels(channels);
}

#include "Quant.hpp"
#include "SnapTrimpot.hpp"

struct QuantWidget : ModuleWidget
{
    QuantWidget(Quant* module)
    {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/Quant.svg")));

        // skinny module.  two screws leaves room for a label
        addChild(createWidget<ScrewSilver>(Vec(0, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        // column centered at 7.622mm

        addParam(createParamCentered<SnapTrimpot>(mm2px(Vec(7.622, 11.500)), module, Quant::PARAM_SCALE));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.622, 22.500)), module, Quant::INPUT_SCALE));

        addParam(createParamCentered<SnapTrimpot>(mm2px(Vec(7.622, 39.625)), module, Quant::PARAM_ROOT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.622, 50.625)), module, Quant::INPUT_ROOT));

        addParam(createParamCentered<SnapTrimpot>(mm2px(Vec(7.622, 67.750)), module, Quant::PARAM_OCTAVE));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.622, 78.750)), module, Quant::INPUT_OCTAVE));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.622, 95.250)), module, Quant::INPUT_PITCH));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.622, 108.414)), module, Quant::OUTPUT_PITCH));
    }

    void appendContextMenu(Menu* menu) override {
        auto m = dynamic_cast<Quant *>(module);
        assert(m);

        menu->addChild(createBoolPtrMenuItem("Relative octave", "", &m->relativeOctave));
        menu->addChild(createBoolPtrMenuItem("Relative root", "", &m->relativeRoot));
    }
};

Model* modelQuant = createModel<Quant, QuantWidget>("Quant");