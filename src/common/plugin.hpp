//  Copyright (c) 2023, 2 Lift Studios
//  All rights reserved.

#pragma once
#include "rack.hpp"

using namespace rack;

// Declare the Plugin, defined in plugin.cpp
extern Plugin* pluginInstance;

// Declare each Model, defined in each module source file
// extern Model* modelMyModule;

extern Model * modelComps;
extern Model * modelMerge;
extern Model * modelPVolt;
extern Model * modelSampleAndHold;
extern Model * modelSplit;
extern Model * modelSteps;
extern Model * modelVCASR;

