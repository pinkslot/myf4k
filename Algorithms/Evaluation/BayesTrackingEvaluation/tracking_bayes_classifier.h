#ifndef TrackingBayesClassifier_nbc_header
#define TrackingBayesClassifier_nbc_header

#include <naive_bayes_classifier.h>

class TrackingBayesClassifier : public NaiveBayesClassifier
{
	public:

	// Constructor
	TrackingBayesClassifier()
	{
		// Set training data
		sample_size = 6;
		class_priors[0] = 0.5;
		class_means[0].push_back(0.778254);
		class_stddevs[0].push_back(0.145264);
		class_means[0].push_back(0.488704);
		class_stddevs[0].push_back(0.165805);
		class_means[0].push_back(1.09259);
		class_stddevs[0].push_back(0.66498);
		class_means[0].push_back(0.407299);
		class_stddevs[0].push_back(0.221756);
		class_means[0].push_back(86.7197);
		class_stddevs[0].push_back(50.8114);
		class_means[0].push_back(138.067);
		class_stddevs[0].push_back(50.0404);
		class_priors[1] = 0.5;
		class_means[1].push_back(0.879262);
		class_stddevs[1].push_back(0.12438);
		class_means[1].push_back(0.863631);
		class_stddevs[1].push_back(0.136766);
		class_means[1].push_back(2.03414);
		class_stddevs[1].push_back(0.678969);
		class_means[1].push_back(0.556269);
		class_stddevs[1].push_back(0.291412);
		class_means[1].push_back(54.0441);
		class_stddevs[1].push_back(50.4533);
		class_means[1].push_back(65.8376);
		class_stddevs[1].push_back(38.6517);
	}
};

#endif
