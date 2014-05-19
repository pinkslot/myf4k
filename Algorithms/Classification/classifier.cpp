#include "classifier.h"
#include "opencv_utils.h"
#include <log.h>
#include <vector>
#include <math.h>
#include <complex>
#include <iostream>
#include <list>
#include <array>
#include <highgui.h>
#include <svm_utils>

#include <histogram.h>

namespace alg
{
	Classifier::Classifier(filling_type ftype, string path) 
	{
		
	}

	void extractAIM(Mat img, array<long double>(6) &ans)
	{
		int img[][3] = {1, 0 ,0 , 1 ,1 , 0};
		compd mean;
		list<compd> points;
		double n = 0;
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 3; j++)
				if (img[i][j] == 1)
				{
					n++;
					compd p = compd(i,j);
					mean += p, points.push_back(p);
				}
		mean /= n;
		compd conjmean = conj(mean);

		array<compd,5> comp_mom;
		for (list<compd>::iterator pp = points.begin(); pp != points.end(); pp++)
		{
			compd c1 = *pp - mean, c2 = conj(*pp) - conjmean;
			comp_mom[0] += pow(c1,1) * pow(c2,1);
			comp_mom[1] += pow(c1,2) * pow(c2,0);
			comp_mom[2] += pow(c1,2) * pow(c2,1);
			comp_mom[3] += pow(c1,1) * pow(c2,2);
			comp_mom[4] += pow(c1,3) * pow(c2,0);
		}

		comp_mom[0] /= pow(n, 2);
		comp_mom[1] /= pow(n, 2);
		comp_mom[2] /= pow(n, 2.5);
		comp_mom[3] /= pow(n, 2.5);
		comp_mom[4] /= pow(n, 2.5);
		ans[0] = real(comp_mom[0]);
		ans[1] = real(compd(1e3,0)*comp_mom[2]*comp_mom[3]);
		ans[2] = 1e4 * real(comp_mom[1]*comp_mom[3]*comp_mom[3]);
		ans[3] = 1e4 * imag(comp_mom[1]*comp_mom[3]*comp_mom[3]);
		ans[4] = 1e6 * real(comp_mom[4]*comp_mom[3]*comp_mom[3]*comp_mom[3]);
		ans[5] = 1e6 * imag(comp_mom[4]*comp_mom[3]*comp_mom[3]*comp_mom[3]);
	}		
	// void Classifier::extractHist(const Mat& img)
	// {
	// 	const int FEATURES_COUNT = 5;
	// 	vector<float> features(FEATURES_COUNT);
	// 	// get some features based on hist monment(mean, )
	// 	Histogram hist(img);
	// 	vector<float> c_moments(3), b_moments(4);
	// 	hist.moments(4, &c_moments[0]);
	// 	b_moments[0] = hist.mean();
	// 	b_moments[1] = c_moments[1] + pow(b_moments[0], 2);
	// 	b_moments[2] = c_moments[2] + 3*b_moments[0]*b_moments[1] - 2*pow(b_moments[0], 3);
	// 	b_moments[3] = c_moments[3] + 4*b_moments[0]*b_moments[2] - 6*pow(b_moments[0], 2)*b_moments[1] + 3*pow(b_moments[0], 4);
	// 	features.push_back(b_moments[0]);							// mean
	// 	features.push_back(c_moments[0]);							// stand dev
	// 	features.push_back(c_moments[1]/pow(c_moments[0], 1.5));	// skew
	// 	features.push_back(c_moments[2]/pow(c_moments[0], 2));		// kurt

	// }
}