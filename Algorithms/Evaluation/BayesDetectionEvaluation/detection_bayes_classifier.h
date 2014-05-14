#ifndef DetectionBayesClassifier_nbc_header
#define DetectionBayesClassifier_nbc_header

#include <naive_bayes_classifier.h>

class DetectionBayesClassifier : public NaiveBayesClassifier
{
	public:

	// Constructor
	DetectionBayesClassifier()
	{
		// Set training data
		sample_size = 16;
		class_priors[0] = 0.5;
		class_means[0].push_back(160.205);
		class_stddevs[0].push_back(92.8152);
		class_means[0].push_back(130.001);
		class_stddevs[0].push_back(60.5938);
		class_means[0].push_back(1035.9);
		class_stddevs[0].push_back(760.576);
		class_means[0].push_back(0.31322);
		class_stddevs[0].push_back(0.222357);
		class_means[0].push_back(0.469229);
		class_stddevs[0].push_back(0.166281);
		class_means[0].push_back(0.542101);
		class_stddevs[0].push_back(0.186358);
		class_means[0].push_back(0.0830848);
		class_stddevs[0].push_back(0.0786137);
		class_means[0].push_back(0.211024);
		class_stddevs[0].push_back(0.209571);
		class_means[0].push_back(0.000597179);
		class_stddevs[0].push_back(0.00025234);
		class_means[0].push_back(9.73927);
		class_stddevs[0].push_back(9.73415);
		class_means[0].push_back(36.2594);
		class_stddevs[0].push_back(20.1699);
		class_means[0].push_back(29.3953);
		class_stddevs[0].push_back(16.7994);
		class_means[0].push_back(0.304781);
		class_stddevs[0].push_back(0.148317);
		class_means[0].push_back(0.310694);
		class_stddevs[0].push_back(0.150793);
		class_means[0].push_back(47.2733);
		class_stddevs[0].push_back(23.383);
		class_means[0].push_back(38.9264);
		class_stddevs[0].push_back(19.9993);
		class_priors[1] = 0.5;
		class_means[1].push_back(154.061);
		class_stddevs[1].push_back(78.7594);
		class_means[1].push_back(88.8676);
		class_stddevs[1].push_back(51.6908);
		class_means[1].push_back(756.806);
		class_stddevs[1].push_back(1028.23);
		class_means[1].push_back(0.666438);
		class_stddevs[1].push_back(0.245577);
		class_means[1].push_back(0.667894);
		class_stddevs[1].push_back(0.212606);
		class_means[1].push_back(0.779776);
		class_stddevs[1].push_back(0.252335);
		class_means[1].push_back(0.180776);
		class_stddevs[1].push_back(0.0727863);
		class_means[1].push_back(0.630106);
		class_stddevs[1].push_back(0.238873);
		class_means[1].push_back(0.00107871);
		class_stddevs[1].push_back(0.000617019);
		class_means[1].push_back(31.3995);
		class_stddevs[1].push_back(21.4554);
		class_means[1].push_back(47.8001);
		class_stddevs[1].push_back(24.0042);
		class_means[1].push_back(40.8905);
		class_stddevs[1].push_back(16.7423);
		class_means[1].push_back(0.427217);
		class_stddevs[1].push_back(0.193444);
		class_means[1].push_back(0.440109);
		class_stddevs[1].push_back(0.197893);
		class_means[1].push_back(36.7663);
		class_stddevs[1].push_back(21.3029);
		class_means[1].push_back(30.3204);
		class_stddevs[1].push_back(16.9359);
	}
};

#endif
