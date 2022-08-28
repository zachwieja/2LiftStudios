#pragma once

#include "plugin.hpp"
#include "Scale.hpp"

struct Quant : Module
{
    public:
        std::vector<std::string> names = {
            "Major",
            "Dorian",
            "Phrygian",
            "Lydian",
            "Myxolodian",
            "Aeolian",
            "Locrian",
            "Acoustic",
            "Altered",
            "Augmented",
            "Bebop dom.",
            "Blues",
            "Chromatic",
            "Enigmatic",
            "Euler-Fokker",
            "Flamenco",
            "Gypsy",
            "Half diminished",
            "Harmonic Major",
            "Harmonic Minor",
            "Hirajoshi",
            "Hungarian",
            "Miyako-bushi",
            "Insen",
            "Iwato",
            "Lydian Augmented",
            "Bebob Major",
            "Locrian Major",
            "Pentatonic Major",
            "melodic Minor",
            "Pentatonic Minor - Yo",
            "Neapoliltan Major",
            "Neapolitan Minor",
            "Octatonic 1",
            "Octatonic 2",
            "Persian",
            "Phrygian dominant",
            "Prometheus",
            "Harmonics",
            "Tritone",
            "Tritone 2S",
            "Ukrainian Dorian",
            "Wholetone",
            "Quartertone"
        };

        std::vector<const char *> intervals = {
            "2212221",
            "2122212",
            "1222122",
            "2221221",
            "2212212",
            "2122122",
            "1221222",
            "2221212",
            "1212222",
            "313131",
            "22122111",
            "321132",
            "111111111111",
            "1322211",
            "23232",
            "1312131",
            "2131122",
            "2121222",
            "2212131",
            "2122131",
            "42141",
            "2131131",
            "14214",
            "14242",
            "14142",
            "2222121",
            "22121211",
            "2211222",
            "22323",
            "2122221",
            "32232",
            "1222221",
            "1222131",
            "21212121",
            "12121212",
            "1311231",
            "1312122",
            "222312",
            "311223",
            "132132",
            "114114",
            "2131212",
            "222222",
            "111111111111111111111111"
        };

    public:
        enum ParamId
        {
            PARAM_ROOT,
            PARAM_SCALE,
            PARAM_OCTAVE,
            PARAMS_LEN
        };

        enum InputId
        {
            INPUT_ROOT,
            INPUT_SCALE,
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
        Scale **  scales;
        int       numScales = 0;
        float     steps = 0;

    public:
        bool      relative  = false;
        const int MAX_CHANNELS = 16;

    public: 
        Quant();
        ~Quant();

        void process(const ProcessArgs &args) override;
        json_t* dataToJson() override;
        void dataFromJson(json_t* root) override;

        float getOctave();
        float getPitch(int channel);
        float getRoot(Scale * scale);
        Scale * getScale();
};
