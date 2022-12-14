//  Copyright (c) 2022, 2 Lift Studios
//  All rights reserved.

#include "plugin.hpp"

Plugin* pluginInstance;

void init(Plugin* p) {
	pluginInstance = p;

	// Add modules here
	p->addModel(modelComps);
	p->addModel(modelMerge);
	p->addModel(modelPVolt);
	p->addModel(modelSampleAndHold);
	p->addModel(modelSplit);
	p->addModel(modelSteps);

	// Any other plugin initialization may go here.
	// As an alternative, consider lazy-loading assets and lookup tables when your module is created to reduce startup times of Rack.
}
