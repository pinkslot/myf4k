#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include <iostream>
#include <log.h>
#include <highgui.h>
#include "../algorithm.h"
#include "histogram.h"

using namespace cv;
using namespace std;

namespace alg
{

	class Classifier : public alg::Algorithm
	{
		public:

		//Methods
		void train(const Mat& img);
		Classifier();		
				
		// Implement algorithm information
		inline string name() const { return "classifier"; }
		inline string type() const { return "classification"; }		
		inline string description() const { return "Ada boost classifier"; }
		inline string version() const { return "1.0"; }
		inline int executionTime() const { return 0; }
		inline int ram() const { return 0; }
		inline string input() const { return "cvb::CvBlobs"; }
		inline string output() const { return "cvb::CvBlobs"; }		
	};

}
#endif