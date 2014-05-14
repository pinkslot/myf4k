// An implementation of a naive Bayesian classifier, using Gaussian distribution for features

#include <naive_bayes_classifier.h>
#include "my_exception.h"
#include "math_utils.h"
#include <iostream>
#include <fstream>
#include <log.h>
	
// Train model.
// train_data_file: file containing the number of features on the first line, the number of samples on the second line, and then the list of samples with the class at the end
void NaiveBayesClassifier::train(string train_data_file, bool prior_as_frequency)
{
	// Open file
	ifstream in_file(train_data_file.c_str());
	if(!in_file.is_open())
	{
		throw MyException("Error opening NBC training file.");
	}
	// Read number of features
	int num_features;
	in_file >> num_features;
	// Read number of samples
	int num_samples;
	in_file >> num_samples;
	// Create matrices
	Mat train_data(num_samples, num_features, CV_32FC1);
	Mat labels(num_samples, 1, CV_32SC1);
	// Read file
	for(int i=0; i<num_samples; i++)
	{
		// Read features
		for(int f=0; f<num_features; f++)
		{
			// Read feature value
			float value;
			in_file >> value;
			Log::debug(0) << "Sample " << i << ", feature " << f << ": " << value << endl;
			// Write to matrix
			train_data.at<float>(i,f) = value;
		}
		// Read label
		int label;
		in_file >> label;
		labels.at<int>(i,0) = label;
	}
	// Train
	train(train_data, labels, prior_as_frequency);
}

// Train model.
// train_data: CV_32FC1 matrix, each row is a sample
// labels: CV_32SC1, integer class numbers
// prior_as_frequenct: compute class prior as occurrence frequency
void NaiveBayesClassifier::train(const Mat& train_data, const Mat& labels, bool prior_as_frequency)
{
	// Check matrices' size and type
	if(train_data.rows != labels.rows)
	{
		throw MyException("Number of training samples must be equal to number of class outputs");
	}
	if(train_data.type() != CV_32FC1)
	{
		throw MyException("Type of training data matrix must be CV_32FC1");
	}
	if(labels.type() != CV_32SC1)
	{
		throw MyException("Type of label matrix must be CV_32SC1");
	}
	if(labels.cols != 1)
	{
		throw MyException("Label matrix can have only one column");
	}
	// Save sample size
	sample_size = train_data.cols;
	Log::debug(0) << "Num samples: " << labels.rows << endl;
	Log::debug(0) << "Sample size: " << sample_size << endl;
	// Set class prior
	for(int i=0; i<labels.rows; i++)
	{
		// Get label
		int label = labels.at<int>(i,0);
		//Log::debug(0) << "Label: " << label << endl;
		// Check if label exists
		if(class_priors.find(label) == class_priors.end())
		{
			Log::debug(0) << "New label: " << label << endl;
			// Create label entry
			class_priors[label] = 0;
		}
		if(prior_as_frequency)
		{
			// Increase class occurrence count
			class_priors[label] += 1.0f/labels.rows;
		}
	}
	if(!prior_as_frequency)
	{
		// Set equiprobable prior
		for(map<int, float>::iterator it = class_priors.begin(); it != class_priors.end(); it++)
		{
			it->second = 1.0f/class_priors.size();
		}
	}
	// Compute distribution parameters for each class and for each feature
	// For each feature...
	for(int f=0; f<train_data.cols; f++)
	{
		// Define structure to divide feature values between classes
		map<int, vector<float> > values;
		// Add all values for this feature to the corresponding class
		for(int i=0; i<train_data.rows; i++)
		{
			// Read value
			float value = train_data.at<float>(i, f);
			// Read class
			int label = labels.at<int>(i, 0);
			//Log::debug(0) << "Sample " << i << ", feature " << f << ": " << value << " (class: " << label << ")" << endl;
			// Add to corresponding vector
			values[label].push_back(value);
		}
		// Once all values have been grouped by class, for each of them compute mean and standard deviation
		for(map<int, vector<float> >::iterator it = values.begin(); it != values.end(); it++)
		{
			// Compute mean and variance
			float mean, stddev;
			MathUtils::computeMeanAndStdDev(it->second, mean, stddev);
			// Add to maps
			class_means[it->first].push_back(mean);
			class_stddevs[it->first].push_back(stddev);
			//cout << "feature " << f << ", class " << it->first << ": " << mean << " " << stddev*stddev << endl;
		}
	}
}

// Classify sample:
// sample: sample to classify
// prediction_degree: probability associated to the best matching class
// prediction_values: map which associates each class to the probability the samples belongs to it)
int NaiveBayesClassifier::classify(const vector<float>& sample, float *prediction_degree, map<int, float>* prediction_values) const
{
	// Build feature select vector
	vector<int> feature_select;
	for(unsigned int f=0; f<sample_size; f++)
	{
		feature_select.push_back(f);
	}
	// Call other function
	return classifySelectFeatures(feature_select, sample, prediction_degree, prediction_values);
}

// Classify sample using only the features identified by the given index array:
// features: 0-based list of indeces which identify the features to be used
// sample: sample to classify
// prediction_degree: probability associated to the best matching class
// prediction_values: map which associates each class to the probability the samples belongs to it)
int NaiveBayesClassifier::classifySelectFeatures(const vector<int>& feature_select, const vector<float>& sample, float *prediction_degree, map<int, float>* prediction_values) const
{
	// Check features vector
	if(feature_select.size() > sample_size)
	{
		throw MyException("Invalid feature-select vector size.");
	}
	// Check sample size
	if(sample.size() != feature_select.size())
	{
		throw MyException("Invalid sample size.");
	}
	// Variables to keep track of best matching class
	int best_class = -1;
	float best_prediction = -1.0f;
	// If prediction_values map has been provided, clear it
	if(prediction_values != NULL)
	{
		prediction_values->clear();
	}
	// Go through each class and compute posterior numerators and evidence (i.e. posterior denumerator)
	map<int, float> post_numerators;
	float evidence = 0.0f;
	for(map<int, float>::const_iterator prior_it = class_priors.begin(); prior_it != class_priors.end(); prior_it++)
	{
		// Get class label
		int label = prior_it->first;
		// Initialize posterior as class prior
		float p = prior_it->second;
		// Multiply by likelihood terms
		int sample_index = 0;
		for(vector<int>::const_iterator f_it = feature_select.begin(); f_it != feature_select.end(); f_it++)
		{
			unsigned int f = *f_it;
			// Check index
			if(f >= sample_size)
			{
				throw MyException("Invalid feature index.");
			}
			map<int, vector<float> >::const_iterator mean_it = class_means.find(label);
			map<int, vector<float> >::const_iterator stddev_it = class_stddevs.find(label);
			p *= gaussianPdf(sample[sample_index++], mean_it->second[f], stddev_it->second[f]);
		}
		// Add to posterior numerators
		post_numerators[label] = p;
		// Sum to evidence
		evidence += p;
	}
	// Normalize based on evidence
	for(map<int, float>::iterator post_it = post_numerators.begin(); post_it != post_numerators.end(); post_it++)
	{
		// Normalize
		post_it->second = post_it->second/evidence;
		// Check if it's the best prediction
		if(post_it->second > best_prediction)
		{
			// Save as best
			best_class = post_it->first;
			best_prediction = post_it->second;
		}
		// If prediction_values map is provided, save value
		if(prediction_values != NULL)
		{
			map<int, float>& prediction_values_ref = *prediction_values;
			prediction_values_ref[post_it->first] = post_it->second;
		}
	}
	// If prediction_degree is provided, write it
	if(prediction_degree != NULL)
	{
		*prediction_degree = best_prediction;
	}
	// Return best matching class
	return best_class;
}

// Implementation of Gaussian PDF
float NaiveBayesClassifier::gaussianPdf(float value, float mean, float stddev) const
{
	return 1.0f/sqrt(2*M_PI*stddev*stddev)*exp(-(value-mean)*(value-mean)/(2*stddev*stddev));
}
	
// Write training data to file
void NaiveBayesClassifier::writeToFile(string file) const
{
	// Open file
	ofstream out_file(file.c_str());
	if(!out_file.is_open())
	{
		throw MyException("Error opening NBC output file.");
	}
	// Write number of features
	out_file << sample_size << endl;
	// Write number of classes
	out_file << class_priors.size() << endl;
	// For each class...
	for(map<int,float>::const_iterator prior_it = class_priors.begin(); prior_it != class_priors.end(); prior_it++)
	{
		// Write class label
		out_file << prior_it->first << endl;
		// Write class prior
		out_file << prior_it->second << endl;
		// For each feature, write the class' mean and std. dev.
		for(unsigned int f=0; f<sample_size; f++)
		{
			map<int, vector<float> >::const_iterator mean_it = class_means.find(prior_it->first);
			map<int, vector<float> >::const_iterator stddev_it = class_stddevs.find(prior_it->first);
			out_file << mean_it->second[f] << " " << stddev_it->second[f] << endl;
		}
	}
	// Close file
	out_file.close();
}
	
// Write training data to file which can be included in C++ code
void NaiveBayesClassifier::writeToClassFile(string file, string class_name) const
{
	// Open file
	ofstream out_file(file.c_str());
	if(!out_file.is_open())
	{
		throw MyException("Error opening NBC output file.");
	}
	// Write class code
	out_file	<< "#ifndef " << class_name << "_nbc_header" << endl
			<< "#define " << class_name << "_nbc_header" << endl
			<< endl
			<< "#include <naive_bayes_classifier.h>" << endl
			<< endl
			<< "class " << class_name << " : public NaiveBayesClassifier" << endl
			<< "{" << endl
			<< "\tpublic:" << endl
			<< endl
			<< "\t// Constructor" << endl
			<< "\t" << class_name << "()" << endl
			<< "\t{" << endl
			<< "\t\t// Set training data" << endl;
	out_file << "\t\tsample_size = " << sample_size << ";" << endl;
	// For each class
	for(map<int,float>::const_iterator prior_it = class_priors.begin(); prior_it != class_priors.end(); prior_it++)
	{
		// Write priors
		out_file << "\t\tclass_priors[" << prior_it->first << "] = " << prior_it->second << ";" << endl;
		// For each feature, write the class' mean and std. dev.
		for(unsigned int f=0; f<sample_size; f++)
		{
			map<int, vector<float> >::const_iterator mean_it = class_means.find(prior_it->first);
			map<int, vector<float> >::const_iterator stddev_it = class_stddevs.find(prior_it->first);
			out_file << "\t\tclass_means[" << prior_it->first << "].push_back(" << mean_it->second[f] << ");" << endl;
			out_file << "\t\tclass_stddevs[" << prior_it->first << "].push_back(" << stddev_it->second[f] << ");" << endl;
		}
	}
	out_file << "\t}" << endl;
	out_file << "};" << endl;
	out_file << endl;
	out_file << "#endif" << endl;
	// Close file
	out_file.close();
}
	
// Read training data from file
void NaiveBayesClassifier::readFromFile(string file)
{
	// Clear structures
	class_priors.clear();
	class_means.clear();
	class_stddevs.clear();
	// Open file
	ifstream in_file(file.c_str());
	if(!in_file.is_open())
	{
		throw MyException("Error opening NBC model file.");
	}
	// Read number of samples
	in_file >> sample_size;
	// Read number of classes
	int num_classes;
	in_file >> num_classes;
	for(int i=0; i<num_classes; i++)
	{
		// Read class label
		int label;
		in_file >> label;
		//cout << "read label: " << label << endl;
		// Read class prior
		float prior;
		in_file >> prior;
		class_priors[label] = prior;
		// For each feature, read class mean and std. dev.
		for(unsigned int f=0; f<sample_size; f++)
		{
			// Read mean
			float mean;
			in_file >> mean;
			class_means[label].push_back(mean);
			// Read std. dev.
			float stddev;
			in_file >> stddev;
			class_stddevs[label].push_back(stddev);
		}
	}
}
