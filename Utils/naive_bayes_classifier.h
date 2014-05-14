// An implementation of a naive Bayesian classifier, using Gaussian distribution for features

#ifndef NAIVE_BAYES_CLASSIFIER_H
#define NAIVE_BAYES_CLASSIFIER_H

#include <cv.h>
#include <map>

using namespace cv;
using namespace std;

/** Implementation of a Naive Bayes classifier.
 */
class NaiveBayesClassifier
{
	protected:
	
	// Distribution information
	map<int, float> class_priors;
	map<int, vector<float> > class_means;
	map<int, vector<float> > class_stddevs;

	// Sample information
	unsigned int sample_size;

	// Implementation of Gaussian PDF
	float gaussianPdf(float value, float mean, float stddev) const;

	public:

	/** Default constructor.
	 */
	inline NaiveBayesClassifier() {}

	/** Destructor.
	 */
	inline virtual ~NaiveBayesClassifier() {}

	/** Train model.
	 * \param train_data Data matrix, type CV_32FC1; each row is a sample.
	 * \param labels Label matrix,type CV_32FC1; must have the same number of rows as train_data.
	 * \param prior_as_frequency If true, set the class priors to the corresponding relative frequency in the labels matrix; otherwise, set priors uniformly distributed.
	 */
	void train(const Mat& train_data, const Mat& labels, bool prior_as_frequency = true);
	
	/** Train model from input file.
	 * \param train_data_file Path to file containing the number of features on the first line, the number of samples on the second one, and then the list of samples with the corresponding class at the end of each line.
	 * \param prior_as_frequency If true, set the class priors to the corresponding relative frequency in the labels matrix; otherwise, set priors uniformly distributed.
	 */
	void train(string train_data_file, bool prior_as_frequency = true);

	/** Classify sample.
	 * \param sample Sample to classify
	 * \param prediction_degree Output value representing the probability associated to the best matching class
	 * \param prediction_values Output map which associates each class to the probability that the sample belongs to it
	 * \returns The index of the best-matching class.
	 */
	int classify(const vector<float>& sample, float *prediction_degree = NULL, map<int, float>* prediction_values = NULL) const;
	
	/** Classify sample using only the features identified by the given index array.
	 * \param features 0-based list of indexes which identify the features to be used (corresponding to the features used in the training).
	 * \param sample Sample to classify
	 * \param prediction_degree Output value representing the probability associated to the best matching class
	 * \param prediction_values Output map which associates each class to the probability that the sample belongs to it
	 * \returns The index of the best-matching class.
	 */
	int classifySelectFeatures(const vector<int>& features, const vector<float>& sample, float *prediction_degree = NULL, map<int, float>* prediction_values = NULL) const;

	/** Write classifier data to text file.
	 * \param file File path.
	 */
	void writeToFile(string file) const;

	/** Write classifier data to file which can be included in C++ code.
	 * \param file File path
	 * \param class_name Generated class name
	 */
	void writeToClassFile(string file, string class_name) const;
	
	/** Read classifier data from text file.
	 * \param file File path.
	 */
	void readFromFile(string file);
	
};

#endif
