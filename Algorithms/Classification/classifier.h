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
		}
		void trainModel(string path);
		void loadModel(string path);
		bool predict(const cvb::CvBlob &blob);
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