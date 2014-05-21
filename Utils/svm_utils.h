#ifndef SVM_UTILS_H
#define SVM_UTILS_H

#include <svm.h>
#include <string>
#include <vector>

class SVMUtils
{	
	public:	
	// SVM model
	struct svm_model* svm;

	// Load SVM
	void load(std::string path);
	void loadBuiltIn();

	// Classify vector
	double classify(const std::vector<float>& input, std::vector<bool> select = std::vector<bool>(), bool probability = true) const;
};

#endif
