#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include <iostream>
#include <log.h>
#include "../algorithm.h"
#include <string>
#include <svm.h>

using namespace cv;
using namespace std;
typedef complex<long double> compd;

namespace alg
{
	enum filling_type {FILL_FROM_FILE, FILL_FROM_IMAGE, FILL_FROM_DB}
	
	class Classifier : public alg::Algorithm
	{
	private:
		void extractAIM(const Mat &mask);
		smv_model model;

	public:

		//Methods	
		Classifier(filling_type ftype, string path = 0);
		// Implement algorithm information
		inline string name() const { return "classifier"; }
		inline string type() const { return "classification"; }		
		inline string description() const { return "One class SVM classifier"; }
		inline string version() const { return "1.0"; }
		inline int executionTime() const { return 0; }
		inline int ram() const { return 0; }
		inline string input() const { return "cvb::CvBlobs, cv::Mat"; }
		inline string output() const { return "bool"; }		
	};

}
#endif