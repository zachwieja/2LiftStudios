#pragma once
#include "plugin.hpp"

struct Scale 
{
    public:
        float   steps;     // number of steps in an octave

    private:
        float * voltages;  // map of voltages for O(1) lookup
        int     halfsteps; // 2 * number of steps
        int     notes;     // number of notes in the scale

    public:
        Scale(const char* intervals);
        ~Scale();

    public:
        float getClosest(float voltage);
};