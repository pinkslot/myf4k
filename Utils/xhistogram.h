// Extended histogram - variable bins
#ifndef XHISTOGRAM_H
#define XHISTOGRAM_H

#include <cv.h>
#include <vector>

using namespace std;
using namespace cv;

/** Histogram class with variable bin number and step.
 */
class XHistogram
{
	/** Friend operator, for output.
	 */
	friend ostream& operator<< (ostream& out, const XHistogram& hist);
	
	private:

	// Bin step
	float bin_step;

	// Num bins
	int num_bins;
	
	// Actual histogram storage
	float* histogram;

	// Allocate
	void allocate(int b);

	public:

	/** Default constructor.
	 */
	XHistogram();
	
	/** Copy constructor.
	 */
	XHistogram(const XHistogram& copy);

	/** Destructor.
	 */
	virtual ~XHistogram();
	
	/** Histogram computation.
	 * \param values List of values for which to compute the histogram.
	 * \param bin_step Histogram bin step
	 * \param min_value Left limit for the bins
	 * \param max_value Right limit for the bins
	 */
	void computeHistogram(const vector<float>& values, float bin_step, float min_value, float max_value);

	/** Get the bin step.
	 * \return Bin step
	 */
	inline int binStep() const { return bin_step; }
	
	/** Get the number of bins.
	 * \return Number of bins
	 */
	inline int numBins() const { return num_bins; }

	/** \name Retrieve a histogram value.
	 * \param bin Bin whose frequency we want to retrieve.
	 * \return The frequency for the input bin.
	 */
	/**@{*/
	inline float get(int bin) const { return histogram[bin]; }
	inline const float& operator[] (const int bin) const { return histogram[bin]; }
	inline float& operator[] (const int bin) { return histogram[bin]; }
	/**@}*/

	/** Compute Chi-squared distance.
	 * \param h1 Input histogram
	 * \param h2 Input histogram
	 * \return The Chi-square distance between the input histograms.
	 */
	static float ChiSquaredDistance(const XHistogram& h1, const XHistogram& h2);
	
};

// Output operator
ostream& operator<< (ostream& out, const XHistogram& hist);

#endif
