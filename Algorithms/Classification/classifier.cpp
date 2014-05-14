#include "classifier.h"
#include "opencv_utils.h"
#include <log.h>
#include <vector>
#include <math.h>
namespace alg
{
	Classifier::Classifier() {}
			
	void Classifier::train(const Mat& img)
	{
		const int FEATURES_COUNT = 5;
		vector<float> features(FEATURES_COUNT);
		// get some features based on hist monment(mean, )
		Histogram hist(img);
		vector<float> c_moments(3), b_moments(4);
		hist.moments(4, &c_moments[0]);
		b_moments[0] = hist.mean();
		b_moments[1] = c_moments[1] + pow(b_moments[0], 2);
		b_moments[2] = c_moments[2] + 3*b_moments[0]*b_moments[1] - 2*pow(b_moments[0], 3);
		b_moments[3] = c_moments[3] + 4*b_moments[0]*b_moments[2] - 6*pow(b_moments[0], 2)*b_moments[1] + 3*pow(b_moments[0], 4);
		features.push_back(b_moments[0]);							// mean
		features.push_back(c_moments[0]);							// stand dev
		features.push_back(c_moments[1]/pow(c_moments[0], 1.5));	// skew
		features.push_back(c_moments[2]/pow(c_moments[0], 2));		// kurt

	}
}