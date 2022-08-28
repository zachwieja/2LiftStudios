//  Copyright (c) 2022, 2 Lift Studios
//  All rights reserved.

#pragma once
#include <rack.hpp>

using namespace rack;

// Declare the Plugin, defined in plugin.cpp
extern Plugin* pluginInstance;

// Declare each Model, defined in each module source file
// extern Model* modelMyModule;

extern Model* modelMerge;
extern Model* modelQuant;
extern Model * modelSplit;
