#include "vibe-background.h"

int main()
{
	// Background model
	vibeModel_t* model;
	// Initialize model
	model = libvibeModelNew();
	//libvibeModelSetMatchingThreshold(model, parameters.get<int>("threshold"));
	//libvibeModelSetMatchingNumber(model, parameters.get<int>("number"));
	libvibeModelPrintParameters(model);
	// Set parameters		
	//libvibeModelSetMatchingThreshold(model, 21);
	//libvibeModelSetMatchingNumber(model, 3);
	//libvibeModelPrintParameters(model);
}
