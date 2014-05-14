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
		sample_size = 13;
		class_priors[0] = 0.5;
		class_means[0].push_back(177.268);
		class_stddevs[0].push_back(117.736);
		class_means[0].push_back(100.86);
		class_stddevs[0].push_back(70.0822);
		class_means[0].push_back(980.62);
		class_stddevs[0].push_back(1170.75);
		class_means[0].push_back(0.421141);
		class_stddevs[0].push_back(0.170532);
		class_means[0].push_back(0.209341);
		class_stddevs[0].push_back(0.134459);
		class_means[0].push_back(0.172842);
		class_stddevs[0].push_back(0.106992);
		class_means[0].push_back(0.162268);
		class_stddevs[0].push_back(0.113861);
		class_means[0].push_back(0.0956762);
		class_stddevs[0].push_back(0.101155);
		class_means[0].push_back(0.0859593);
		class_stddevs[0].push_back(0.149019);
		class_means[0].push_back(4.62734);
		class_stddevs[0].push_back(2.89734);
		class_means[0].push_back(0.191165);
		class_stddevs[0].push_back(0.132629);
		class_means[0].push_back(0.191412);
		class_stddevs[0].push_back(0.134336);
		class_means[0].push_back(16.759);
		class_stddevs[0].push_back(2.73472);
		class_priors[1] = 0.5;
		class_means[1].push_back(186.221);
		class_stddevs[1].push_back(126.707);
		class_means[1].push_back(106.78);
		class_stddevs[1].push_back(77.1622);
		class_means[1].push_back(1270.11);
		class_stddevs[1].push_back(2381.51);
		class_means[1].push_back(0.638467);
		class_stddevs[1].push_back(0.213483);
		class_means[1].push_back(0.362535);
		class_stddevs[1].push_back(0.212177);
		class_means[1].push_back(0.255736);
		class_stddevs[1].push_back(0.152361);
		class_means[1].push_back(0.161627);
		class_stddevs[1].push_back(0.0694726);
		class_means[1].push_back(0.0700229);
		class_stddevs[1].push_back(0.0601284);
		class_means[1].push_back(0.415091);
		class_stddevs[1].push_back(0.318992);
		class_means[1].push_back(4.60676);
		class_stddevs[1].push_back(3.83758);
		class_means[1].push_back(0.402252);
		class_stddevs[1].push_back(0.196815);
		class_means[1].push_back(0.40476);
		class_stddevs[1].push_back(0.202046);
		class_means[1].push_back(13.4461);
		class_stddevs[1].push_back(3.71731);
	}
};

#endif
