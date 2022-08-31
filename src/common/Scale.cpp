//  Copyright (c) 2022, 2 Lift Studios
//  All rights reserved.

#include "Scale.hpp"

Scale::Scale(const char* intervals)
{
    assert(intervals != NULL && intervals[0] != '\0');

    // the number of intervals is the number  of  notes

    this->notes = this->steps = 0;

    // iterate over intervals string. convert the ascii
    // digits to integers and sum to get number of steps

    for (const char* d = intervals; *d != '\0'; d++) 
    {
        unsigned int digit = *d - '0';
        assert(digit <= 9);

        this->notes += 1;
        this->steps += digit;
    }

    // allocate the voltage array for all the halfsteps

    this->halfsteps = this->steps * 2;
    this->voltages = (float *) calloc(this->halfsteps, sizeof(float));

    // go back to the beginning of the interval  string
    // and build the arrays by iterating over intervals

    int i = 0, steps = 0;
    float step = 0.0f;

    for (int j = 1; j <= this->notes * 2; j++) 
    {
        if (j % 2)
            steps = intervals[j / 2] - '0';
        else {
            step += steps;
        }

        for (int k = 0; k < steps; k++) {
            this->voltages[i++] = step / this->steps;
        }
    }
}

Scale::~Scale()
{
    if (this->voltages) {
        free(this->voltages);
        this->voltages = NULL;
    }
}

float Scale::getClosest(float voltage) 
{
    // cheap floor() is safe for our small voltage range
    // and  we  know the voltage is clamped to + or - 5V

    int octave = (int)(voltage + 100.0f) - 100;

    // remainder is the pitch within the octave,  do not
    // use abs().  conditional negation is  much  faster

    float pitch = voltage - octave;
    if (pitch < 0) pitch = -pitch;

    // scale pitch to halfsteps and truncate for  lookup

    int index = (int)(pitch * this->halfsteps);
    return octave + this->voltages[index];
}
