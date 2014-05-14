#include <naive_bayes_classifier.h>
#include <iostream>
#include "my_exception.h"
#include <log.h>
//#include "tracking_eval_model.h"

int main()
{
	try
	{
		Log::setDebugLogStream(cout);
		/*Mat train_data(8, 3, CV_32FC1);
		Mat_<float>& train_data_fill = (Mat_<float>&) train_data;
		train_data_fill << 6, 180, 12, 5.92, 190, 11, 5.58, 170, 12, 5.92, 165, 10, 5, 100, 6, 5.5, 150, 8, 5.42, 130, 7, 5.75, 150, 9;
		Mat labels(8, 1, CV_32SC1);
		Mat_<int>& labels_fill = (Mat_<int>&) labels;
		labels_fill << 0, 0, 0, 0, 1, 1, 1, 1;
		cout << train_data << endl;
		cout << labels << endl;*/
		NaiveBayesClassifier nbc;
		nbc.train("train_certainties.txt", false);
		//nbc.train(train_data, labels, false);*/
		//NBCTest nbc_test;
		//nbc.readFromFile("nbc.txt");
		/*vector<float> sample;
		sample.push_back(6);
		sample.push_back(130);
		sample.push_back(8);
		float best_prediction;
		map<int, float> prediction_map;*/
		//vector<int> feature_select;
		//feature_select.push_back(0);
		//feature_select.push_back(1);
		//feature_select.push_back(2);
		//nbc.classifySelectFeatures(feature_select, sample, &best_prediction, &prediction_map);
		//nbc_test.classify(sample, &best_prediction, &prediction_map);
		//for(map<int, float>::iterator it = prediction_map.begin(); it != prediction_map.end(); it++)
		//{
		//	cout << "class " << it->first << ": " << it->second << endl;
		//}
		nbc.writeToClassFile("detection_bayes_classifier.h", "DetectionBayesClassifier");
		//nbc.writeToFile("detection_eval_model.txt");
	}
	catch(MyException& e)
	{
		cout << e.what() << endl;
	}

}
