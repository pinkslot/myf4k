#include "classifier.h"
#include <vector>
#include <math.h>
#include <complex>

#include <list>
#include <highgui.h>
#include <log.h>
#include <histogram.h>
#include <iostream>
// TODO fix bag with load model without mean dev value
namespace alg
{
	vector<svm_node> Classifier::extractAIM(Mat img)
	{
		if (img.channels()==4 || img.channels()==3)
			cvtColor(img, img, CV_BGR2GRAY);			
		threshold(img, img, 100, 255, 0);	
		compd mean;
		list<compd> points;
		double n = 0;
		for (int i = 0; i != img.rows; i++)
			for (int j = 0; j !=  img.cols; j++)
				if (img.at<uchar>(i,j) == 255)
				{
					n++;
					compd p = compd(i+1,j+1);
					mean += p, points.push_back(p);
				}
		mean /= n;
		compd conjmean = conj(mean);
		vector<compd> comp_mom(5);
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

		vector<svm_node> ans(7);	
		ans[0].value = real(comp_mom[0]);
		ans[1].value = real(compd(1e3,0)*comp_mom[2]*comp_mom[3]);
		ans[2].value = 1e4 * real(comp_mom[1]*comp_mom[3]*comp_mom[3]);
		ans[3].value = 1e4 * imag(comp_mom[1]*comp_mom[3]*comp_mom[3]);
		ans[4].value = 1e6 * real(comp_mom[4]*comp_mom[3]*comp_mom[3]*comp_mom[3]);
		ans[5].value = 1e6 * imag(comp_mom[4]*comp_mom[3]*comp_mom[3]*comp_mom[3]);
		for (int i = 0; i < 6; i++)
			ans[i].index = i+1;
		ans[6].index = -1;
		ans[6].value = -1;
		return ans;
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

	bool Classifier::predict(const Mat &mask)
	{
		// return anytime true if not init 
		if (!model) return true;

		vector<svm_node> feature = extractAIM(mask);
		for (unsigned i = 0; i < feature.size(); i++)
			feature[i].value = (feature[i].value-mean[i]) / dev[i];

		for (unsigned i = 0; i < feature.size()-1; i++)
			cout << mean[i] << " " << dev[i] << endl;
		svm_save_model("qwe", model);
		double res = svm_predict(model, feature.data());
		return res > 0;
	}


	void Classifier::trainModel(string path)
	{
		if ( !boost::filesystem::exists(path) ) 
			throw MyException("Not such directory");
		boost::filesystem::directory_iterator end; // default construction yields past-the-end

		// vector pfeatures for store vector of each features, vector features - for store poiter to it's data
		vector<svm_node *> features;
		vector< vector<svm_node> > pfeatures;

		for (boost::filesystem::directory_iterator itr(path); itr != end; itr++)
		{
			string fname = itr->path().filename().string();
			// each files pair should have such name: <fname>src.bmp, <fname>msk.bmp
			int suff_idx = fname.size()-7;
			if (fname.size() < 7) 
				continue;
			string full_sec_fname =  path + fname.substr(0, suff_idx) + "msk.bmp";
			Mat msk;
			if (fname.substr(suff_idx) == string("src.bmp") && boost::filesystem::exists(full_sec_fname))
				{
					/*Log::info(0)*/cout <<  "train classifier on: " << full_sec_fname << endl;
					msk = imread(full_sec_fname, CV_LOAD_IMAGE_GRAYSCALE);
					if (!msk.data) 
						throw MyException("Cannot read file" + full_sec_fname);  	 					
					// prepare mask to extract features 

  				pfeatures.push_back(extractAIM(msk));			
  			}
		}
		unsigned feature_count = pfeatures.size(), feature_size = pfeatures[0].size() - 1;
		mean = vector<double>(feature_size, 0), dev = vector<double>(feature_size, 0);
		for (unsigned i  = 0; i < feature_count; i++)
		{
			features.push_back(pfeatures[i].data());
			for (unsigned j = 0; j < feature_size; j++)
				mean[j] += pfeatures[i][j].value, dev[j] += pfeatures[i][j].value * pfeatures[i][j].value;
		}
		cout << "mean " << "dev" << endl;
		for (unsigned j = 0; j < feature_size; j++)
		{
			mean[j] /= feature_count;
			dev[j] = sqrt(dev[j]/feature_count - mean[j]*mean[j]);
			cout << mean[j] << " " << dev[j] << endl;
		}

		// normalize
		for (unsigned i  = 0; i < feature_count; i++)
			for (unsigned j = 0; j < feature_size; j++)
				pfeatures[i][j].value = (pfeatures[i][j].value-mean[j]) / dev[j];

		vector<double> label(features.size(), 1);			
		svm_problem prob = {features.size(), label.data(), features.data()};			
		svm_parameter param = {ONE_CLASS, RBF};
		param.gamma = 0.4;
		param.nu =  0.002;			
		param.eps = 1e-6;
		model = svm_train(&prob, &param);
		string model_fname = path + "model.svm";
		/*Log::info(0) */cout <<  "save model as " << model_fname << endl;
		svm_save_model(model_fname.c_str(), model);
		SVMUtils ut;
		ut.load(model_fname);
		model = ut.svm;	
	}		

	void Classifier::loadModel(string path)
	{
		if ( !boost::filesystem::exists(path) ) 
			throw MyException("Not such directory");		
		path += "model.svm";
		SVMUtils ut;
		ut.load(path);
		model = ut.svm;
	}		
}