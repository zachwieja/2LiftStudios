//  Copyright (c) 2022, 2 Lift Studios
//  All rights reserved.

#include "Scale.hpp"

Scale::~Scale()
{
    if (this->voltages) {
        free(this->voltages);
        this->voltages = NULL;
    }
}

void Scale::initialize(const char * name, const char * intervals)
{
    // if the name is too long,  then we truncate (no big deal)

    if (name == NULL || name[0] == '\0') throw "Scale name missing or empty.";

    strncpy(this->name, name, sizeof(this->name) - 1);
    this->name[sizeof(this->name) - 1] = '\0';

    // but if the interval string is too long,  then we  cannot
    // complete the construction and we must throw an exception

    if (intervals == NULL || intervals[0] == '\0') throw "Scale intervals missing or empty.";
    if (strlen(intervals) > sizeof(this->intervals) - 1) throw "Too many scale intervals";

    strncpy(this->intervals, intervals, sizeof(this->intervals) - 1);
    this->intervals[sizeof(this->intervals) - 1] = '\0';

    // count the intervals  (the  number of  notes)  and  steps

    this->notes = this->steps = 0;

    // iterate over intervals string.  convert the ascii digits
    // to integers and then sum digits to get number  of  steps

    for (const char * d = intervals; *d != '\0'; d++)
    {
        unsigned int digit = *d - '0';
        if (digit > 9) throw "Scale intervals contains invalid character";

        this->notes += 1;
        this->steps += digit;
    }

    // allocate the voltage array.  double the number of steps
    // so we can easily quantize the value  between two  notes

    this->halfsteps = this->steps * 2;
    this->voltages = (float *) calloc(this->halfsteps, sizeof(float));

    // go back to the beginning of  the  intervals  string  and
    // calculate the voltages  we  will  use  for  quantization

    int i = 0, steps = 0;
    float step = 0.0f;

    for (int j = 1; j <= this->notes * 2; j++)
    {
        if (j % 2)
            steps = intervals[j / 2] - '0';
        else {
            step += steps;
        }

        // copy voltage multiple times every  half-step

        for (int k = 0; k < steps; k++) {
            this->voltages[i++] = step / this->steps;
        }
    }
}


float Scale::getClosest(float voltage) 
{
    // cheap floor() is safe for our small voltage range and we
    // know the voltage is clamped to +- 5V, shift and truncate

    int octave = (int) (voltage + 100.0f) - 100;

    // remainder is the pitch within the octave, do not use abs
    // it does too much.  conditional negation is  much  faster

    float pitch = voltage - octave;
    if (pitch < 0) pitch = -pitch;

    // scale pitch to halfsteps and truncate to  produce  index

    int index = (int) (pitch * this->halfsteps);
    return octave + this->voltages[index];
}
