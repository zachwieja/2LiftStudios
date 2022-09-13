//  Copyright (c) 2022, 2 Lift Studios
//  All rights reserved.

#pragma once
#include "plugin.hpp"

struct Scale 
{
    public:
        char    name[32];         // name of scale used in labels
        char    intervals[64];    // intervals used to calculate steps
        float   steps;            // number of steps in an octave

    private:
        float * voltages = NULL;  // map of voltages for O(1) lookup
        int     halfsteps;        // 2 * number of steps
        int     notes;            // length of the interval string

    public:
        ~Scale();

        float getClosest(float voltage);
        void initialize(const char * name, const char * intervals);
};