#include "detection_chooser.h"
#include "gaussian_mixture_model.h"
#include "IModel.h"
#include "wave_back.h"
#include "xml_detection.h"
#include "vibe2.h"
#include "apmm2.h"
#include "CodeBook.h"
#include "ada.h"
#include "xbe.h"
#include <AdaptiveMedianBGS.h>
#include <Eigenbackground.h>
#include <GrimsonGMM.h>
#include <MeanBGS.h>
#include <PratiMediodBGS.h>
#include <WrenGA.h>
#include <ZivkovicAGMM.h>
#include <MultiLayerBGS.h>
#include <algorithm>

using namespace std;

namespace alg
{
	// Initialize static field
	vector<string> DetectionChooser::valid_names;

	// Get list of names
	vector<string> DetectionChooser::list()
	{
		// Check if list is empty
		if(valid_names.empty())
		{
			// Initialize valid names
			valid_names.push_back("agmm");
			valid_names.push_back("apmm2");
			valid_names.push_back("cb");
			valid_names.push_back("gmm");
			valid_names.push_back("im");
			valid_names.push_back("vibe2");
			valid_names.push_back("wb");
			valid_names.push_back("ada");
			valid_names.push_back("xbe");
			valid_names.push_back("am");
			valid_names.push_back("eigen");
			valid_names.push_back("ggmm");
			valid_names.push_back("mean");
			valid_names.push_back("pm");
			valid_names.push_back("wren");
			valid_names.push_back("zgmm");
			valid_names.push_back("ml");
			valid_names.push_back("xml");
		}
		// Return names
		return valid_names;
	}

	// Create algorithm from name
	Detection* DetectionChooser::create(string detection_alg)
	{
		// Make sure list if filled
		list();
		// Check name exists
		if(find(valid_names.begin(), valid_names.end(), detection_alg) == valid_names.end())
		{
			stringstream error;
			error << "Invalid detection algorithm '" << detection_alg << "'.";
			throw MyException(error.str());
		}
		// Instantiate algorithm
		Detection *detection;
		if(detection_alg  == "gmm" || detection_alg  == "agmm")
		{
			detection = new GaussianMixtureModel();
		}
		else if(detection_alg == "im")
		{
			detection = new IModel();
		}
		else if(detection_alg == "cb")
		{
			detection = new CodeBook();
		}
		else if(detection_alg == "wb")
		{
			detection = new wave_back();
		}
		else if(detection_alg == "vibe2")
		{
			detection = new ViBE2();
		}
		else if(detection_alg == "apmm2")
		{
			detection = new APMM2();
		}
		else if(detection_alg == "ada")
		{
			detection = new AdaBoost();
		}
		else if(detection_alg == "xbe")
		{
			detection = new Xbe();
		}
		else if(detection_alg == "am")
		{
			detection = new AdaptiveMedian();
		}
		else if(detection_alg == "eigen")
		{
			detection = new Eigenbackground();
		}
		else if(detection_alg == "ggmm")
		{
			detection = new GrimsonGMM();
		}
		else if(detection_alg == "mean")
		{
			detection = new MeanBGS();
		}
		else if(detection_alg == "pm")
		{
			detection = new PratiMediod();
		}
		else if(detection_alg == "wren")
		{
			detection = new WrenGA();
		}
		else if(detection_alg == "zgmm")
		{
			detection = new ZivkovicAGMM();
		}
		else if(detection_alg == "ml")
		{
			detection = new MultiLayerBGS();
		}
		else if(detection_alg == "xml")
		{
			detection = new XMLDetection();
		}
		else
		{
			stringstream error;
			error << "Invalid detection algorithm '" << detection_alg << "'.";
			throw MyException(error.str());
		}
		// Return algorithm
		return detection;
	}

}

