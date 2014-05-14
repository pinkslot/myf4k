#ifndef OPENCV_UTILS_H
#define OPENCV_UTILS_H

#include <cv.h>
#include <highgui.h>
#include <cvblob.h>
#include <object.h>
// Debug includes
#include <iostream>

// Define blob drawing modes
#define CV_POSITION_ORIGINAL	0
#define CV_POSITION_TOP_LEFT	1

/** \file opencv_utils.h
 */

/** Convert BGR to YIQ color space.
 * \param bgr BGR input image
 * \returns YIQ converted image
 */
cv::Mat cvtColorYIQ(const cv::Mat& bgr);

/** Draw multiline.
 * \param points List of points of the multiline
 * \param frame Frame on which to draw
 * \param color Line color
 * \param thickness Line thickness in pixels
 */
void drawMultiLine(const std::vector<cv::Point>& points, cv::Mat& frame, cv::Scalar color, int thickness = 1);

/** Draw single contour.
 * \param points List of contour points
 * \param frame Frame on which to draw
 * \param color Line color
 * \param thickness_or_filled Line thickness in pixels, CV_FILLED to fill the contour's interior
 */
void drawContour(const std::vector<cv::Point>& points, cv::Mat& frame, cv::Scalar color, int thickness_or_filled = 1);

/** Point comparison function, for contour comparison.
 * \param point_1 Input point
 * \param point_2 Input point
 * \returns True if the first point is either closer to the left margin or the top margin of the image, false otherwise.
 */
bool comparePoints(const cv::Point point_1, const cv::Point point_2);

// Compare contour: returns true if they're equal
bool compareContours(const std::vector<cv::Point>& contour1, const std::vector<cv::Point>& contour2);
bool compareContours(const cvb::CvBlob& contour1, const cvb::CvBlob& contour2);

// Compute bounding box from contour
std::vector<cv::Point> boundingBoxFromContour(const std::vector<cv::Point>& contour);

// Check a video is valid
bool isVideoValid(cv::VideoCapture& cap, float* certainty = NULL, int* frame_count = NULL, int sampling_rate = 300, float max_prob_threshold = 0.5f);
		
// Check blob is valid
bool isBlobValid(const cvb::CvBlob& blob, unsigned int max_width = INT_MAX, unsigned int max_height = INT_MAX);

// Show blob
void showBlob(const std::string&, const cvb::CvBlob& blob, int position = CV_POSITION_ORIGINAL, bool filled = true, int output_width = -1, int output_height = -1);

// Show object
void showObject(const std::string&, const pr::Object& object);

// Compute min and max coordinates from point list
void minMaxCoords(const std::vector<cv::Point>&, int* min_x, int* max_x, int* min_y, int* max_y);

// Compute common boundary
cv::Mat commonBoundary(const cvb::CvBlob& blob1, const cvb::CvBlob& blob2);

// Compute cohesiveness strength between two patches
float cohesivenessStrength(const cvb::CvBlob& patch_1, const cvb::CvBlob& patch_2);

// Compute boundary complexity
float boundaryComplexity(const cv::Mat& patch_mask, const std::vector<cv::Point>& c, int s, int k, int sn);

// Wrapper for cvLabel
cvb::CvBlobs computeBlobs(const cv::Mat& frame);

// Compute superpixels straddling score
float superpixelsStraddlingScore(const cv::Mat& seg, const cv::Mat& mask);

// Superpixel segmentation
void segmentSuperpixels(const cv::Mat& frame, cv::Mat& seg, int& num_sp, int k = 500, int min = 20);

// Create mask pictures from blob
cv::Mat blobToMat(const cvb::CvBlob& blob, int position = CV_POSITION_TOP_LEFT, bool filled = true, int output_width = -1, int output_height = -1);

// Get centroid point from point vector
cv::Point centroid(const std::vector<cv::Point>& points);

// Get center point from point vector
cv::Point center(const std::vector<cv::Point>& points);

// Compute distance between points
inline float pointDistance(const cv::Point& p1, const cv::Point& p2)
{
	return sqrt((p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y));
}

// Compute sum between numbers with wrapping to 0 after maximum value (excluded)
inline int wrap(int a, int m)
{
	// Apply modified mod function
	return ((a % m) + m) % m;
}

// Create CvBlob from point vectors
cvb::CvBlob* createBlob(const std::vector<cv::Point>& bounding_box, const std::vector<cv::Point>& contour);
cvb::CvBlob* createBlob(const std::vector<cv::Point>& contour);

// Draw a set of blobs
cv::Mat rebuildBlobs(const cvb::CvBlobs& blobs, int width, int height);

// Compute list of points between two points
std::vector<cv::Point> getPointsBetween(const cv::Point& p1, const cv::Point& p2);

cv::Mat drawBlob(const cvb::CvBlob& blob, bool filled = true, int output_width = -1, int output_height = -1);
//Mat drawBlob2(const cvb::CvBlob& blob, bool filled = true, int mode = CV_DRAW_BLOB_MIN, int output_width = -1, int output_height = -1, int* padding = NULL);
void drawBlobContour(cv::Mat& frame, const cvb::CvBlob& blob, cv::Scalar color = CV_RGB(255,255,0));
cv::Mat contourBlob(cvb::CvBlob& blob);
IplImage *rebuildBlob(cvb::CvBlob& blob);
bool inNeighbourhood(cv::Point point_1, cv::Point point_2);
int sign(float f);
std::vector<cv::Point> fixContour(const std::vector<cv::Point>& contour);
//std::vector<cv::Point> fixContour2(const std::vector<cv::Point>& contour);
void blobToBoundingBoxAndContour(const cvb::CvBlob& blob, std::vector<cv::Point>& bounding_box, std::vector<cv::Point>& contour);
unsigned char pointDifferenceToChainCode(cv::Point last, cv::Point next);
std::vector<cv::Point> blobToPointVector(const cvb::CvBlob& blob);
int binaryArea(const cv::Mat& img);

// Co-occurrence matrix:
// - image: input image
// - position: offset between the two pixels to correlate
// - num_bins: size of output matrix
// - normalize: divide occurence number by total sum
cv::Mat computeCoOccurrenceMatrix(const cv::Mat& image, cv::Point position, int num_bins, bool normalize = true);

// Limited-size buffer with a least-recently-used deletion policy.
// Items are indexed from 0, where 0 is the most recently inserted element.
template<typename T>
class CircularBuffer
{
	private:

	// Queue memory
	T* buffer;
	// Position of the last element written
	int head;
	// Number of elements already written
	int num_elements;
	// Maximum size
	int size;

	public:

	// Constructor:
	// - size: maximum number of elements
	CircularBuffer(int size_) : head(-1), num_elements(0), size(size_)
	{
		// Allocate memory
		buffer = new T[size];
	}

	// Get number of elements
	inline int numElements() { return num_elements; }

	// Get maximum size
	inline int maxSize() { return size; }

	// Add copy of given element to the buffer
	void add(const T& item)
	{
		// Increment head counter
		head = (head + 1) % size;
		// Add a copy to the array
		buffer[head] = item;
		// Increment number of elements
		num_elements = (num_elements == size ? num_elements : num_elements+1);
	}

	// Get item:
	// - index: position in the array, where 0 is the most recent element
	T& get(int index)
	{
		/*cout << "asked: " << index << endl;
		cout << "num elems: " << num_elements << endl;
		cout << "size: " << size << endl;
		cout << "head: " << head << endl;*/
		index = head - index;
		if(index < 0)
		{
			index = size + index;
		}
		//cout << "returning: " << index << endl;
		return buffer[index];
	}

	// Destructor, frees buffer
	virtual ~CircularBuffer()
	{
		delete [] buffer;
	}

};

#endif
