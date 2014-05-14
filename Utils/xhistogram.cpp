#include "xhistogram.h"
#include "my_exception.h"
#include "shell_utils.h"
// Debug includes
#include <iostream>

using namespace std;

// Allocate memory
void XHistogram::allocate(int num_bins)
{
	// Allocate memory
	histogram = new float[num_bins];
	// Set to zero
	for(int i=0; i<num_bins; i++)
		histogram[i] = 0;
}
	
// Constructor
// - values: input values
XHistogram::XHistogram()
{
	// Set histogram pointer
	histogram = NULL;
}
	
// Copy constructor
XHistogram::XHistogram(const XHistogram& copy)
{
	throw MyException("Histogram copy not supported");
}

// Destructor
XHistogram::~XHistogram()
{
	// Check pointer
	if(histogram != NULL)
		delete [] histogram;
}

// Actual histogram computation
void XHistogram::computeHistogram(const vector<float>& values, float bin_step, float min_value, float max_value)
{
	// Free histogram, if necessary
	if(histogram != NULL) delete [] histogram;
	// Count values
	int num_values = values.size();
	// Set bin step
	this->bin_step = bin_step;
	// Get min and max
	//float min_value = INT_MAX, max_value = INT_MIN;
	//for(vector<float>::const_iterator it = values.begin(); it != values.end(); it++)
	//{
	//	if((*it) < min_value)	min_value = *it;
	//	if((*it) > max_value)	max_value = *it;
	//}
	//cout << "min: " << min_value << endl;
	//cout << "max: " << max_value << endl;
	// Modify data so that the results are the same as in Matlab
	min_value = min_value - bin_step/2;
	num_bins = 0;
	float tmp = min_value;
	while(tmp < max_value)
	{
		num_bins++;
		tmp += bin_step;
	}
	// Count bins
	//num_bins = ceil((max_value - min_value)/bin_step);
	// Allocate
	allocate(num_bins);
	// Check each value
	for(vector<float>::const_iterator it = values.begin(); it != values.end(); it++)
	{
		// Get value
		float value = *it;
		// Find bin index
		int bin = floor((value - min_value)/bin_step);
		// Check limit
		if(bin >= num_bins)
		{
			// Put in last bin
			bin = num_bins - 1;
		}
		// Increase count
		histogram[bin] += 1;
	}
	// Normalize histogram
	for(int k=0; k<num_bins; k++)
	{
		histogram[k] /= num_values;
	}
	//cout << histogram << endl;
}

// Compute Chi-squared distance
float XHistogram::ChiSquaredDistance(const XHistogram& h1, const XHistogram& h2)
{
	// Check histograms
	if(h1.num_bins != h2.num_bins || h1.bin_step != h2.bin_step)
	{
		throw MyException("Invalid histograms");
	}
	float sum = 0.0f;
	for(int k=0; k<h1.num_bins; ++k)
	{
		if(h1[k] != h2[k])
		{
			sum += (h1[k] - h2[k])*(h1[k] - h2[k])/(h1[k] + h2[k]);
		}
	}
	return sum/2;
}

// Output operator
ostream& operator<< (ostream& out, const XHistogram& hist)
{
	int bins = hist.numBins();
	if(bins == 0)
	{
		out << "[]";
		return out;
	}
	out << "[" << hist.histogram[0];
	for(int i=1; i<bins; i++)
	{
		out << ", " << hist.histogram[i];
	}
	out << "]";
	return out;
}
