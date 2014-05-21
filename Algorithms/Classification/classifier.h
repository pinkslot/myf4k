#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include "../algorithm.h"
#include <string>
#include <svm.h>
#include <opencv_utils.h>
#include <svm_utils.h>
#include <boost/filesystem.hpp>
#include <my_exception.h>
#include <cvblob.h>

using namespace cv;
using namespace std;
typedef complex<double> compd;

namespace alg
{
	class Classifier : public alg::Algorithm
	{
	private:
		vector<svm_node> extractAIM(Mat mask);
		svm_model *model;
		// save mean and dev of train set to normalize features 
		vector<double> mean, dev;	
	public:

		//Methods	
		Classifier(): model(0)
		{
			// init default value;
			mean = vector<double>(6), dev = vector<double>(6);
			mean[0] = 0.395421, mean[1] = 8.69947, mean[2] = 45.229, 
				mean[3] = -2.17732, mean[4] = 421.798, mean[5] = -6.69579;
			dev[0] = 0.130585, dev[1] = 17.6471, dev[2] = 122.05,
				dev[3] = 38.4359, dev[4] = 2259.9, dev[5] = 131.137;
		}
		void trainModel(string path);
		void loadModel(string path);
		bool predict(const Mat &mask);

		// Implement algorithm information
		inline string name() const { return "classifier"; }
		inline string type() const { return "classification"; }		
		inline string description() const { return "One class SVM classifier, \
													this classifier require only positive \
													example for train"; }
		inline string version() const { return "1.0"; }
		inline int executionTime() const { return 0; }
		inline string descriptionExecutionTime() const { return "TODO"; }
		inline int ram() const { return 0; }
		inline string input() const { return "cvb::CvBlobs, cv::Mat"; }
		inline string output() const { return "bool"; }		
	};
}
#endif