#include "opencv_utils.h"
#include "histogram.h"
#include <my_exception.h>
#include <blob_processing.h>
#include <segment-image.h>
#include <cmath>
#include <log.h>
#include <sstream>
// Debug includes
#include <highgui.h>
#include <iostream>

using namespace std;
using namespace cv;

// Convert BGR to YIQ color space
Mat cvtColorYIQ(const Mat& bgr)
{
	// Convert to 32 bit
	//cout << "bgr = " << bgr << endl << endl;
	Mat bgrf;
	bgr.convertTo(bgrf, CV_32F, 1.0/255.0);
	//cout << "bgrf = " << bgrf << endl << endl;
	// Convert to [R1 G1 B1 ; R2 G2 B2 ; ... ; Rn Gn Bn] format
	Mat reshaped(bgrf.rows*bgrf.cols, 3, CV_32F);
	for(int i=0; i<bgrf.rows; ++i)
	{
		for(int j=0; j<bgrf.cols; ++j)
		{
			// Get pixel
			Vec3f& pixel = bgrf.at<Vec3f>(i,j);
			// Write pixel as a row in the reshaped matrix
			reshaped.at<float>(i*bgrf.cols+j, 0) = pixel[2];
			reshaped.at<float>(i*bgrf.cols+j, 1) = pixel[1];
			reshaped.at<float>(i*bgrf.cols+j, 2) = pixel[0];
		}
	}
	//cout << "reshaped: " << reshaped << endl << endl;
	// Define conversion matrix
	Mat conversion = (Mat_<float>(3,3) << 0.299, 0.595716, 0.211456, 0.587, -0.274453, -0.522591, 0.114, -0.321263, 0.311135);
	//cout << "conversion = " << conversion << endl << endl;
	// Convert single channels
	Mat yiqf_reshaped = reshaped*conversion;
	//cout << "yiqf_reshaped = " << yiqf_reshaped << endl << endl;
	// Scale each column back to the 0-255 range
	yiqf_reshaped.col(0) = yiqf_reshaped.col(0)*255;
	yiqf_reshaped.col(1) = (yiqf_reshaped.col(1) + 0.5957)*214.033909686;
	yiqf_reshaped.col(2) = (yiqf_reshaped.col(2) + 0.5226)*243.097244546;
	//cout << "yiqf_reshaped 0-244 = " << yiqf_reshaped << endl << endl;
	// Convert to CV_8U
	Mat yiq_reshaped;
	yiqf_reshaped.convertTo(yiq_reshaped, CV_8U);
	//cout << "yiq_reshaped = " << yiq_reshaped << endl << endl;
	// Reshape back to the original size
	Mat yiq(bgr.rows, bgr.cols, CV_8UC3);
	for(int n=0; n<yiq_reshaped.rows; n++)
	{
		// Get row
		uchar* row = yiq_reshaped.ptr<uchar>(n);
		// Compute coordinates
		int i = n / bgr.cols;
		int j = n % bgr.cols;
		// Write value
		yiq.at<Vec3b>(i,j) = Vec3b(row[0], row[1], row[2]);
	}
	// Return result
	return yiq;
}

// Draw multiline
void drawMultiLine(const std::vector<cv::Point>& points, cv::Mat& frame, cv::Scalar color, int thickness)
{
	// Check number of points
	if(points.size() < 2)
	{
		return;
	}
	// Starting point
	Point start = points[0];
	// Draw following points
	for(vector<Point>::const_iterator it = points.begin()+1; it != points.end(); it++)
	{
		// Draw segment
		line(frame, start, *it, color, thickness);
		// Update start point
		start = *it;
	}
}

// Compare points, for contour comparison
bool comparePoints(const cv::Point point_1, const cv::Point point_2)
{
	// Check x coordinates
	if(point_1.x < point_2.x)	return true;
	else if(point_2.x < point_1.x)	return false;
	// If the x'es are the same, check the y coordinate
	if(point_1.y < point_2.y)	return true;
	else				return false;
}

// Compare contour: returns true if they're equal
bool compareContours(const vector<Point>& contour_1, const vector<Point>& contour_2)
{
	// Get unique points
	vector<Point> unique_1, unique_2;
	for(vector<Point>::const_iterator c_it = contour_1.begin(); c_it != contour_1.end(); c_it++)
	{
		if(find(unique_1.begin(), unique_1.end(), *c_it) == unique_1.end())
		{
			unique_1.push_back(*c_it);
		}
	}
	for(vector<Point>::const_iterator c_it = contour_2.begin(); c_it != contour_2.end(); c_it++)
	{
		if(find(unique_2.begin(), unique_2.end(), *c_it) == unique_2.end())
		{
			unique_2.push_back(*c_it);
		}
	}
	// Check length
	if(unique_1.size() != unique_2.size())
	{
		return false;
	}
	// Sort contours
	sort(unique_1.begin(), unique_1.end(), comparePoints);
	sort(unique_2.begin(), unique_2.end(), comparePoints);
	// Compare each point
	for(unsigned int i=0; i<unique_1.size(); ++i)
	{
		if(unique_1[i] != unique_2[i])
		{
			// Difference found, return false
			return false;
		}
	}
	// No difference found, return true
	return true;
}

bool compareContours(const cvb::CvBlob& blob1, const cvb::CvBlob& blob2)
{
	// Convert blobs to contour
	vector<Point> bounding_box_1, bounding_box_2;
	vector<Point> contour_1, contour_2;
	blobToBoundingBoxAndContour(blob1, bounding_box_1, contour_1);
	blobToBoundingBoxAndContour(blob2, bounding_box_2, contour_2);
	// Call overloaded function
	return compareContours(contour_1, contour_2);
}

// Compute bounding box from contour
vector<Point> boundingBoxFromContour(const vector<Point>& contour)
{
	// Initialize minimum and maximum coordinates
	int min_x = INT_MAX, min_y = INT_MAX, max_x = INT_MIN, max_y = INT_MIN;
	// Go through each point
	for(vector<Point>::const_iterator it = contour.begin(); it != contour.end(); ++it)
	{
		if(it->x < min_x)	min_x = it->x;
		if(it->y < min_y)	min_y = it->y;
		if(it->x > max_x)	max_x = it->x;
		if(it->y > max_y)	max_y = it->y;
	}
	// Create and fill output vector
	vector<Point> bounding_box;
	bounding_box.push_back(Point(min_x, min_y));
	bounding_box.push_back(Point(max_x, min_y));
	bounding_box.push_back(Point(max_x, max_y));
	bounding_box.push_back(Point(min_x, max_y));
	bounding_box.push_back(Point(min_x, min_y));
	// Return result
	return bounding_box;
}

// Check a video is valid
bool isVideoValid(VideoCapture& cap, float* certainty, int* frame_count, int sampling_rate, float max_prob_threshold)
{
	// Initialize frame count
	int frames = 0;
	// Initialize counter for valid/invalid sample frames
	int good_frames = 0;
	int bad_frames = 0;
	// Start processing frames
	while(true)
	{
		// Frame buffer
		Mat frame;
		// Read frame
		cap >> frame;
		// Check end of video
		if(frame.rows == 0 || frame.cols == 0)
		{
			// Stop cycle
			break;
		}
		// Check sampling rate
		if((frames % sampling_rate) == 0)
		{
			// Analyze frame
			// Convert to grayscale
			Mat gray;
			cvtColor(frame, gray, CV_BGR2GRAY);
			// Compute histogram
			Histogram hist(gray);
			// Get maximum probability
			float max_prob = hist.maxProb();
			// Increment corresponding counter
			if(max_prob > max_prob_threshold)	bad_frames++;
			else					good_frames++;
		}
		// Increase frame count
		frames++;
	}
	// Check if we need to write frame count
	if(frame_count != NULL)
	{
		*frame_count = frames;
	}
	// Check result
	bool result = good_frames > bad_frames;
	int num_samples = good_frames + bad_frames;
	Log::debug(0) << "Checking video validity: " << result << " (good frames: " << good_frames << ", bad frames: " << bad_frames;
	// Check if we need to write the certainty
	if(certainty != NULL)
	{
		if(result == true)	*certainty = ((float)good_frames)/((float)num_samples);
		else			*certainty = ((float)bad_frames)/((float)num_samples);
		Log::debug(0) << ", " << *certainty;
	}
	Log::debug(0) << ")" << endl;
	// Return result
	return result;
}

// Check blob is valid
bool isBlobValid(const cvb::CvBlob& blob, unsigned int max_width, unsigned int max_height)
{
	// Check minimum coordinates
	if(blob.minx < 0 || blob.miny < 0)
	{
		Log::debug(0) << "BlobProcessing: invalid minx (" << blob.minx << ") or miny (" << blob.miny << ")" << endl;
		return false;
	}
	// Check maximum coordinates
	if(blob.maxx >= max_width || blob.maxy >= max_height)
	{
		Log::debug(0) << "BlobProcessing: invalid maxx (" << blob.maxx << ", max: " << max_width << ") or maxy (" << blob.maxy << ", max: " << max_height << ")" << endl;
		return false;
	}
	// Check contour is included in boundaries
	// Get starting point
	Point current = blob.contour.startingPoint;
	//cout << "start: " << current.x << " " << current.y << endl;
	// Check starting point
	if(current.x < (int) blob.minx || current.x > (int) blob.maxx || current.y < (int) blob.miny || current.y > (int) blob.maxy)
	{
		Log::debug(0) << "BlobProcessing: invalid starting point (" << current.x << ", " << current.y << "), maxx: " << blob.maxx << ", maxy: " << blob.maxy << endl;
		return false;
	}
	// Check all other points
	bool bad_point_found = false;
	const cvb::CvChainCodes& chain_code = blob.contour.chainCode;
	for(cvb::CvChainCodes::const_iterator it=chain_code.begin(); it != chain_code.end(); it++)
	{
		// Compute next point
		current.x += cvb::cvChainCodeMoves[*it][0];
		current.y += cvb::cvChainCodeMoves[*it][1];
		//cout << "next: " << current.x << " " << current.y << endl;
		// Check point
		if(current.x < (int) blob.minx || current.x > (int) blob.maxx || current.y < (int) blob.miny || current.y > (int) blob.maxy)
		{
			bad_point_found = true;
			Log::debug(0) << "BlobProcessing: invalid point (" << current.x << ", " << current.y << "), minx: " << blob.x << ", miny: " << blob.y << ", maxx: " << blob.maxx << ", maxy: " << blob.maxy << endl;
			// Break loop
			break;
		}
	}
	// Check if any bad points have been found
	if(bad_point_found)
	{
		return false;
	}
	// Ok
	return true;
}

// Show object
void showObject(const string& window, const pr::Object& object)
{
	// Create blob
	cvb::CvBlob* blob = createBlob(object.getContour());
	// Show blob
	showBlob(window, *blob);
	// Free blob
	delete blob;
}

// Show blob
void showBlob(const string& window, const cvb::CvBlob& blob, int position, bool filled, int output_width, int output_height)
{
	// Draw blob
	Mat blob_mat = blobToMat(blob, position, filled, output_width, output_height);
	// Show image
	imshow(window, blob_mat);
}

// Compute min and max coordinates from point list
void minMaxCoords(const vector<Point>& points, int* min_x, int* max_x, int* min_y, int* max_y)
{
	*min_x = INT_MAX;
	*max_x = INT_MIN;
	*min_y = INT_MAX;
	*max_y = INT_MIN;
	for(vector<Point>::const_iterator p_it = points.begin(); p_it != points.end(); p_it++)
	{
		//cout << "point: " << p_it->x << ", " << p_it->y << endl;
		if(p_it->x < *min_x)	*min_x = p_it->x;
		if(p_it->x > *max_x)	*max_x = p_it->x;
		if(p_it->y < *min_y)	*min_y = p_it->y;
		if(p_it->y > *max_y)	*max_y = p_it->y;
		//cout << "current mins, maxs: " << *min_x << " " << *max_x << " " << *min_y << " " << *max_y << endl;
	}
}

// Compute cohesiveness strength between two patches
float cohesivenessStrength(const cvb::CvBlob& patch_1, const cvb::CvBlob& patch_2)
{
	// Compute lambda
	float alpha = 20.0f;
	float beta = 3.0f;
	// Compute boundary lengths
	int L_1 = patch_1.contour.chainCode.size();
	int L_2 = patch_2.contour.chainCode.size();
	Mat common_boundary = commonBoundary(patch_1, patch_2);
	int L_12 = binaryArea(common_boundary);
	// Compute lambda
	float lambda = beta*exp(-alpha*L_12/(L_1+L_2));
	// Return result
	return lambda;
}

// Compute common boundary
Mat commonBoundary(const cvb::CvBlob& blob1, const cvb::CvBlob& blob2)
{
	// Render blob2, without filling
	int width = (blob1.maxx > blob2.maxx ? blob1.maxx : blob2.maxx) + 3;
	int height = (blob1.maxy > blob2.maxy ? blob1.maxy : blob2.maxy) + 3;
	Mat mask2 = blobToMat(blob2, CV_POSITION_ORIGINAL, false, width, height);
	// Convert mask1 to point vector
	vector<Point> contour1 = blobToPointVector(blob1);
	// Initialize common boundary image
	Mat common = Mat::zeros(height, width, CV_8UC1);
	// Each contour point
	for(vector<Point>::iterator it = contour1.begin(); it != contour1.end(); ++it)
	{
		// Get point
		Point p = *it;
		// Check if we have a neighbouring contour point in mask2
		int n_x = (p.x > 0 ? p.x-1 : p.x);
		int n_y = (p.y > 0 ? p.y-1 : p.y);
		int n_h = (n_y+2 < height ? 3 : height-n_y);
		int n_w = (n_x+2 < width ? 3 : width-n_x);
		Mat neighbourhood = mask2(Rect(n_x, n_y, n_w, n_h));
		if(binaryArea(neighbourhood) > 0)
		{
			common.at<uchar>(p.y, p.x) = 255;
		}
	}
	// Return common boundary
	return common;
}

// Compute boundary complexity
float boundaryComplexity(const Mat& patch_mask, const vector<Point>& contour, int s, int k, int sn)
{
	// Remove last point if it's the same as the first one
	vector<Point> c = contour;
	if(c[0] == c[c.size()-1])
	{
		c.erase(c.end()-1);
	}
	// Initialize result
	float B = 0.0f;
	// Get number of points
	int N = c.size();
	//cout << "N: " << N << endl;
	// Compute notch map
	bool* notch_map = new bool[N];
	for(int i=0; i<N; i++)	notch_map[i] = 0;
	// Process each point and look for notches
	for(int j=0; j<N; j += sn)
	{
		// Get current, previous and next points
		Point curr = c[j];
		Point prev = c[wrap(j-sn, N)];
		Point next = c[wrap(j+sn, N)];
		// Compute vectors
		Point v1 = prev - curr;
		Point v2 = next - curr;
		// Compute angle
		float cross = abs(v1.x*v2.y - v1.y*v2.x);
		float dot = v1.x*v2.x + v1.y*v2.y;
		float angle = abs(fastAtan2(cross, dot));
		if(angle > 180.0f) angle -= 180.0f;
		// Check is points are more or less aligned
		if(angle < 30.0f || angle > 150.0f)
		{
			// Skip
			continue;
		}
		// Get average point between the two extemities
		Point2f mid((prev.x+next.x)/2.0f, (prev.y+next.y)/2.0f);
		int mid_xs[] = {(int)floor(mid.x), (int)ceil(mid.x)};
		int mid_ys[] = {(int)floor(mid.y), (int)ceil(mid.y)};
		Point cand_1(mid_xs[0], mid_ys[0]);
		Point cand_2(mid_xs[0], mid_ys[1]);
		Point cand_3(mid_xs[1], mid_ys[0]);
		Point cand_4(mid_xs[1], mid_ys[1]);
		if     (cand_1 != prev && cand_1 != curr && cand_1 != next)	mid = cand_1;
		else if(cand_2 != prev && cand_2 != curr && cand_2 != next)	mid = cand_2;
		else if(cand_3 != prev && cand_3 != curr && cand_3 != next)	mid = cand_3;
		else if(cand_4 != prev && cand_4 != curr && cand_4 != next)	mid = cand_4;
		else continue; // Could not decide, skip point
		// Check if mid is inside or outside the object
		if(patch_mask.at<uchar>(mid.y, mid.x) == 0)
		{
			// Notch found
			notch_map[j] = 1;
		}
	}
	// Process each point and look for notches
	for(int d=0; d<N; d++)
	{
		// Compute A_sk
		//cout << "p_d: " << c[d].x << "," << c[d].y << endl;
		//cout << "p_d_ks: " << c[wrap(d+k*s,N)].x << "," << c[wrap(d+k*s,N)].y << endl;
		float A_num = pointDistance(c[d], c[wrap(d+s*k,N)]);
		float A_den = 0.0f;
		for(int j=1; j<=k; ++j)
		{
			float distance_to_next = pointDistance(c[wrap(d+j*s,N)], c[wrap(d+(j-1)*s,N)]);
			A_den += distance_to_next;
		}
		float A = 1.0f - A_num/A_den;
		//cout << "A: " << A << endl;
		// Count number of notches in the window
		float n = 0;
		// Compute notch window interval
		int n_start = d;
		int n_end = d+k*s;
		// Compute number of notches
		for(int i=n_start; i<=n_end; i++)
		{
			// Wrap i
			int wi = wrap(i,N);
			// Check notch
			if(notch_map[wi])
			{
				// Add notch
				n++;
			}
		}
		// Compute F_sk
		float F = 1.0f - 2.0f*abs(0.5 - n/(N-3));
		// Add to B
		B += A*F;
	}
	// Free notch map
	delete [] notch_map;
	// Compute final value for B
	B /= N;
	// Return result
	return B;
}

// Wrapper for cvLabel
cvb::CvBlobs computeBlobs(const Mat& frame)
{
	// Check frame is 8-bit single-channel
	if(frame.channels() != 1 || frame.depth() != CV_8U)
	{
		throw MyException("Input image must be 8-bit single-channel.");
	}
	// Compute blobs
	IplImage *out = cvCreateImage(cvSize(frame.cols, frame.rows), IPL_DEPTH_LABEL, 1);
        IplImage ipl_frame = frame;
        cvb::CvBlobs blobs;
        cvLabel(&ipl_frame, out, blobs);
	// Free stuff
	cvReleaseImage(&out);
	// Return blobs
	return blobs;
}

// Create blob from point vector
cvb::CvBlob* createBlob(const vector<Point>& contour)
{
	// Compute bounding box
	vector<Point> bounding_box = boundingBoxFromContour(contour);
	// Call other function
	return createBlob(bounding_box, contour);
}

// Calls fixContour() on contour
// Both bounding_box and contour can have the last point equal to che first point
cvb::CvBlob* createBlob(const vector<Point>& bounding_box, const vector<Point>& contour)
{
	// Check both arrays are non-empty
	if(bounding_box.size() == 0 || contour.size() == 0)
	{
		throw MyException("Bounding box/contour vectors can't be empty");
	}
	// Allocate blob
	cvb::CvBlob *blob = new cvb::CvBlob();
	// Compute bounding box borders
	int minx = INT_MAX, maxx = 0, miny = INT_MAX, maxy = 0;
	for(vector<Point>::const_iterator it = bounding_box.begin(); it != bounding_box.end(); it++)
	{
		//cout << it->x << " " << it->y << endl;
		if(it->x < minx)	minx = it->x;
		if(it->x > maxx)	maxx = it->x;
		if(it->y < miny)	miny = it->y;
		if(it->y > maxy)	maxy = it->y;
	}
	// Check bounding box coordinates
	if(minx < 0 || miny < 0 || maxx < 0 || maxy < 0)
	{
		throw MyException("Bounding box has negative coordinates");
	}
	blob->minx = minx;
	blob->maxx = maxx;
	blob->miny = miny;
	blob->maxy = maxy;
	// Fix contour
	vector<Point> fixed_contour = fixContour(contour);
	// Compute chaincode contour
	cvb::CvContourChainCode cc_contour;
	cc_contour.startingPoint.x = contour[0].x;
	cc_contour.startingPoint.y = contour[0].y;
	Point last = contour[0];
	for(vector<Point>::iterator it = fixed_contour.begin()+1; it != fixed_contour.end(); it++)
	{
		//cout << "c point: " << it->x << " " << it->y << endl;
		// Check points are different
		if(*it != last)
		{
			// Get corresponding chain code
			uchar code = pointDifferenceToChainCode(last, *it);
			//cout << "prev: (" << last.x << ", " << last.y << "), next: (" << it->x << ", " << it->y << "), code: " << (int)code << endl;
			// Add to contour
			cc_contour.chainCode.push_back(code);
			// Save last point
			last = *it;
		}
	}
	// Set contour chain code
	blob->contour = cc_contour;
	// Compute blob properties
	blob->area = 0;
	blob->m10 = 0;
	blob->m01 = 0;
	blob->m11 = 0;
	blob->m20 = 0;
	blob->m02 = 0;
	Mat blob_mat = blobToMat(*blob);
	for(int i=0; i<blob_mat.rows; ++i)
	{
		for(int j=0; j<blob_mat.cols; ++j)
		{
			if(blob_mat.at<uchar>(i,j) > 0)
			{
				blob->area++;
				blob->m10 += j;
				blob->m01 += i;
				blob->m11 += i*j;
				blob->m20 += j*j;
				blob->m02 += i*i;
			}
		}
	}
	blob->centroid.x = blob->x + blob->m10/blob->area;
	blob->centroid.y = blob->y + blob->m01/blob->area;
	// TODO FIXME compute also other properties
	// Return blob
	return blob;
}

// Get centroid point from point vector
Point centroid(const vector<Point>& points)
{
	// Fix contour
	vector<Point> contour = fixContour(points);
	// Initialize minimum and maximum coordinates
	int avg_x = 0;
	int avg_y = 0;
	// Go through all points
	for(vector<Point>::const_iterator p_it = contour.begin(); p_it != contour.end(); p_it++)
	{
		avg_x += p_it->x;
		avg_y += p_it->y;
	}
	// Return center
	return Point(avg_x/contour.size(), avg_y/contour.size());
}

// Get center point from point vector
Point center(const vector<Point>& points)
{
	// Initialize minimum and maximum coordinates
	int min_x = INT_MAX, min_y = INT_MAX;
	int max_x = 0, max_y = 0;
	// Go through all points
	for(vector<Point>::const_iterator p_it = points.begin(); p_it != points.end(); p_it++)
	{
		if(p_it->x < min_x)	min_x = p_it->x;
		if(p_it->y < min_y)	min_y = p_it->y;
		if(p_it->x > max_x)	max_x = p_it->x;
		if(p_it->y > max_y)	max_y = p_it->y;
	}
	// Return center
	return Point((min_x+max_x)/2, (min_y+max_y)/2);
}

// Draw contour
void drawContour(const vector<Point>& points, Mat& frame, Scalar color, int thickness_or_filled)
{
	// Check contour
	vector<Point> contour = fixContour(points);
	// Build contour vector
	vector<vector<Point> > contours;
	contours.push_back(contour);
	// Call OpenCv drawing function
	drawContours(frame, contours, 0, color, thickness_or_filled);//CV_FILLED, 8);
	/*if(frame.channels() != 3)
	{
		throw MyException("drawContour: expected 3-channel image.");
	}
	// Go through each point
	for(vector<Point>::const_iterator it = points.begin(); it != points.end(); ++it)
	{
		// Get pixel reference
		Vec3b& pixel = frame.at<Vec3b>(it->y, it->x);
		// Set value
		pixel[0] = color[0];
		pixel[1] = color[1];
		pixel[2] = color[2];
	}*/
}

Mat blobToMat(const cvb::CvBlob& blob, int position, bool filled, int output_width, int output_height)
{
	// Check blob is valid
	if(!isBlobValid(blob))
	{
		throw MyException("Can't convert CvBlob to Mat, invalid blob");
	}
	// Compute height and width of the output image size (add 1 padding pixel, otherwise drawContours won't work)
	if(position == CV_POSITION_ORIGINAL)
	{
		if((output_width > 0 && (unsigned int) output_width <= blob.maxx) || (output_height > 0 && (unsigned int) output_height <= blob.maxy))
		{
			stringstream error;
                        error << "Output width (" << output_width << ") or height (" << output_height << ") too small for blob ([" << blob.x << "-" << blob.maxx << ", " << blob.y << "-" << blob.maxy << "]).";        
                        throw MyException(error.str());
		}
	}
	else if(position == CV_POSITION_TOP_LEFT)
	{
		if((output_width > 0 && (unsigned int) output_width < blob.width()) || (output_height > 0 && (unsigned int) output_height < blob.height()))
		{
			stringstream error;
			error << "Output width (" << output_width << ") or height (" << output_height << ") too small for blob (" << blob.width() << "x" << blob.height() << ").";
			throw MyException(error.str());
		}
	}
	else
	{
		throw MyException("Invalid blob position value.");
	}
	int width = (output_width > 0 ? output_width : (position == CV_POSITION_ORIGINAL ? blob.maxx + 1 : blob.width())) + 2;
	int height = (output_height > 0 ? output_height : (position == CV_POSITION_ORIGINAL ? blob.maxy + 1 : blob.height())) + 2;
	// Create output image
	Mat drawn = Mat::zeros(height, width, CV_8UC3);
	// Compute offsets from top-left corner (with padding pixel)
	int x_offset = (position == CV_POSITION_ORIGINAL ? 0 : blob.minx) - 1;
	int y_offset = (position == CV_POSITION_ORIGINAL ? 0 : blob.miny) - 1;
	// Draw blob contour
	// Get starting point
	Point current = blob.contour.startingPoint;
	drawn.at<Vec3b>(current.y - y_offset, current.x - x_offset) = Vec3b(255,255,255);
	// Get chain code contour
	const cvb::CvChainCodes& chain_code = blob.contour.chainCode;
	for(cvb::CvChainCodes::const_iterator it=chain_code.begin(); it != chain_code.end(); it++)
	{
		current.x += cvb::cvChainCodeMoves[*it][0];
		current.y += cvb::cvChainCodeMoves[*it][1];
		drawn.at<Vec3b>(current.y - y_offset, current.x - x_offset) = Vec3b(255,255,255);
	}
	if(filled)
	{
		// Fill contour
		vector<vector<Point> > contours;
		Mat drawn_gs;
		cvtColor(drawn, drawn_gs, CV_BGR2GRAY);
		findContours(drawn_gs, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
		for(unsigned int i=0; i<contours.size(); i++)
		{
			drawContours(drawn, contours, i, CV_RGB(255,255,255), -1);//CV_FILLED, 8);
		}
	}
	// Convert to 8-bit
	Mat final;
	cvtColor(drawn, final, CV_BGR2GRAY);
	// Remove padding pixel
	Rect selection(1, 1, width-2, height-2);
	Mat selected = final(selection);
	// Return result
	return selected;
}

// Compute the superpixels straddling score
float superpixelsStraddlingScore(const cv::Mat& seg, const cv::Mat& mask_in)
{
	// Go through all pixels, group superpixels into single images, and at the same time check for overlap with object's blob
	//Mat mask = blobToMat(blob, CV_POSITION_ORIGINAL, true, seg.cols, seg.rows);
	// Make sure mask is grayscale
	Mat mask;
	if(mask_in.channels() > 1)
	{
		cvtColor(mask_in, mask, CV_BGR2GRAY);
	}
	else
	{
		mask = mask_in.clone();
	}
	// Initialize superpixels mask map; each superpixel is indexed by a number computed from the RGB value used in the segmentation image
	map<int, Mat> sp_masks;
	map<int, bool> sp_straddling;
	// Go through each pixel
	for(int i=0; i<seg.rows; ++i)
	{
		for(int j=0; j<seg.cols; ++j)
		{
			// Get pixel
			const Vec3b& pixel = seg.at<Vec3b>(i,j);
			// Convert it to a number
			int sp = ((int) pixel[0]) + (((int) pixel[1]) << 8) + (((int) pixel[2]) << 16);
			// Check if the corresponding mask has been initialized
			if(sp_masks.find(sp) == sp_masks.end())
			{
				// Add black matrix
				sp_masks[sp] = Mat::zeros(seg.rows, seg.cols, CV_8UC1);
			}
			// Set the pixel in the corresponding mask
			sp_masks[sp].at<uchar>(i,j) = 255;
			// Check if it's straddling
			if(mask.at<uchar>(i,j) > 0)
			{
				// Add to straddling list
				sp_straddling[sp] = true;
			}
		}
	}
	// Initialize score
	float score = 1.0f;
	// Go through all straddling pixels
	for(map<int,bool>::iterator sp_it = sp_straddling.begin(); sp_it != sp_straddling.end(); ++sp_it)
	{
		// Get superpixel mask
		Mat& sp_mask = sp_masks[sp_it->first];
		// Compute areas
		float area_in = binaryArea(sp_mask & mask);
		float area_out = binaryArea(sp_mask & (~mask));
		float area_mask = binaryArea(mask);
		// Update score
		score -= min(area_in, area_out)/area_mask;
	}
	// Return score
	return score;
}
	
void segmentSuperpixels(const Mat& frame, Mat& seg, int& num_sp, int sp_k, int sp_min)
{
	// Initialize output image
	seg.create(frame.rows, frame.cols, CV_8UC3);
	// Convert frame to image<rgb>
	image<rgb> img(frame.cols, frame.rows);
	for(int i=0; i<frame.rows; ++i)
	{
		for(int j=0; j<frame.cols; ++j)
		{
			// Get frame pixel
			const Vec3b& pixel_src = frame.at<Vec3b>(i,j);
			// Get reference to img pixel
			rgb& pixel_dst = imRef((&img), j, i);
			// Copy values
			pixel_dst.r = pixel_src[2];
			pixel_dst.g = pixel_src[1];
			pixel_dst.b = pixel_src[0];
		}
	}
	// Perform segmentation
	image<rgb>* img_seg = segment_image(&img, 0.5, sp_k, sp_min, &num_sp);
	// Convert seg to Mat
	for(int i=0; i<frame.rows; ++i)
	{
		for(int j=0; j<frame.cols; ++j)
		{
			// Get reference to img_seg pixel
			rgb& pixel_src = imRef(img_seg, j, i);
			// Get destination pixel
			Vec3b& pixel_dst = seg.at<Vec3b>(i,j);
			// Copy values
			pixel_dst[2] = pixel_src.r;
			pixel_dst[1] = pixel_src.g;
			pixel_dst[0] = pixel_src.b;
		}
	}
	// Free segmented image
	delete img_seg;	
}

int sign(float f)
{
	if(f > 0) return 1;
	if(f < 0) return -1;
	return 0;
}

bool inNeighbourhood(Point point_1, Point point_2)
{
	return abs(point_1.x-point_2.x) <= 1 && abs(point_1.y-point_2.y) <= 1;
}

unsigned char pointDifferenceToChainCode(Point last, Point next)
{
	int dx = next.x - last.x;
	int dy = next.y - last.y;
	if(dx == 0 && dy == -1)		return CV_CHAINCODE_UP;
	if(dx == 1 && dy == -1)		return CV_CHAINCODE_UP_RIGHT;
	if(dx == 1 && dy == 0)		return CV_CHAINCODE_RIGHT;
	if(dx == 1 && dy == 1)		return CV_CHAINCODE_DOWN_RIGHT;
	if(dx == 0 && dy == 1)		return CV_CHAINCODE_DOWN;
	if(dx == -1 && dy == 1)		return CV_CHAINCODE_DOWN_LEFT;
	if(dx == -1 && dy == 0)		return CV_CHAINCODE_LEFT;
	if(dx == -1 && dy == -1)	return CV_CHAINCODE_UP_LEFT;
	stringstream error;

	error << "Trying to get chain-code of non adjacent points ([ " << last.x << " " << last.y << "], [" << next.x << " " << next.y << "]).";
	throw MyException(error.str());
}

vector<Point> getPointsBetween(const Point& p1, const Point& p2)
{
	// Check if they are in a 3x3 neighbourhood
	if(inNeighbourhood(p1, p2))
	{
		return vector<Point>();
	}
	// Get the point midway
	Point mid((p1.x + p2.x)/2, (p1.y + p2.y)/2);
	// Get the list of points between p1 and mid, and between mid and p2
	vector<Point> p1_mid_points = getPointsBetween(p1, mid);
	vector<Point> mid_p2_points = getPointsBetween(mid, p2);
	// Join all points
	p1_mid_points.push_back(mid);
	p1_mid_points.insert(p1_mid_points.end(), mid_p2_points.begin(), mid_p2_points.end());
	// Return points
	return p1_mid_points;
}

vector<Point> fixContour(const vector<Point>& contour)
{
	vector<Point> new_contour;
	Point start = contour[0];
	Point last = start;
	new_contour.push_back(last);
	unsigned int i = 1;
	while(i < contour.size())
	{
		Point next = contour[i];
		if(next == last)
		{
			i++;
			continue;
		}
		if(inNeighbourhood(last, next))
		{
			new_contour.push_back(next);
			last = next;
			i++;
		}
		else
		{
			//cout << "point 1: " << last.x << " " << last.y << endl;
			//cout << "point 2: " << next.x << " " << next.y << endl;
			// Get points between
			vector<Point> between = getPointsBetween(last, next);
			//for(vector<Point>::iterator it = between.begin(); it != between.end(); it++)
			//	cout << "between: " << it->x << " " << it->y << endl;
			new_contour.insert(new_contour.end(), between.begin(), between.end());
			//cout << "new_contour now:" << endl;
			//for(vector<Point>::iterator it = new_contour.begin(); it != new_contour.end(); it++)
			//	cout << it->x << " " << it->y << endl;
			new_contour.push_back(next);
			last = next;
			i++;
		}
		if(last == start)
		{
			return new_contour;
		}
	}
	if(last != start)
	{
		// Get points between
		vector<Point> between = getPointsBetween(last, start);
		new_contour.insert(new_contour.end(), between.begin(), between.end());
	}
	return new_contour;
}

Mat drawBlob(const cvb::CvBlob& blob, bool filled, int output_width, int output_height)
{
	// Compute height and width of the minimum image size required (plus some margin) to contain the blob, in absolute frame coordinates,
	// or use the given values, if provided
	int width = (output_width > 0 ? output_width : blob.maxx + 2);
	int height = (output_height > 0 ? output_height : blob.maxy + 2);
	Mat drawn = Mat::zeros(height, width, CV_8UC3);
	Mat filtered = Mat::zeros(height, width, CV_8UC3);
	// Get IplImage header, required by cvRenderContourChainCode
	IplImage drawn_ipl = drawn;
	// Do some stuff which fills the contour
	cvb::cvRenderContourChainCode(&(blob.contour), &drawn_ipl, CV_RGB(255,255,255));
	cvtColor(drawn, filtered, CV_RGB2GRAY);
	if(!filled)
	{
		// Crop the blob
		//Rect box(blob.x, blob.y, blob.width(), blob.height());
		Mat final = filtered;
		//imshow("in drawBlob", final);
		//waitKey(0);
		return final;
	}
	//cout << filtered.rows << " " << filtered.cols << endl;
	//imshow("aa", filtered); waitKey(0);
	vector<vector<Point> > contours;
	findContours(filtered, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	drawContours(drawn, contours, -1, CV_RGB(255,255,255), CV_FILLED, 8);
	cvtColor(drawn, filtered, CV_RGB2GRAY);
	Mat median = filtered;
	medianBlur(filtered, median, 3);
	// Crop the blob
	//Rect box(blob.x, blob.y, blob.width(), blob.height());
	Mat final = median;
	//cvReleaseImage(&drawn_ipl);
	return final;
}

int binaryArea(const Mat& img)
{
	// Check channel number
	if(img.channels() > 1)
	{
		throw MyException("binaryArea(): input must be single-channel image.");
	}
	// Compute area, i.e. number of elements greater than 0
	int area = 0;
	for(int i=0; i<img.rows; i++)
	{
		for(int j=0; j<img.cols; j++)
		{
			if(img.at<uchar>(i,j) > 0)
			//if(*(img.data + img.step[0]*i + img.step[1]*j) > 0)
			{
				area++;
			}
		}
	}
	// Return result
	return area;
}
	
vector<Point> blobToPointVector(const cvb::CvBlob& blob)
{
	// Define output vector
	vector<Point> contour;
	// Get starting point
	Point current = blob.contour.startingPoint;
	// Get chain code contour
	const cvb::CvChainCodes& chain_code = blob.contour.chainCode;
	// Add points
	contour.push_back(current);
	//int num_points = chain_code.size();
	for(cvb::CvChainCodes::const_iterator it=chain_code.begin(); it != chain_code.end(); it++)
	{
		current.x += cvb::cvChainCodeMoves[*it][0];
		current.y += cvb::cvChainCodeMoves[*it][1];
		contour.push_back(current);
	}
	// Return result
	return contour;
}

void blobToBoundingBoxAndContour(const cvb::CvBlob& blob, vector<Point>& bounding_box, vector<Point>& contour)
{
	// Clear bounding box
	bounding_box.clear();
	// Add bounding box points
	bounding_box.push_back(Point(blob.minx, blob.miny));
	bounding_box.push_back(Point(blob.maxx, blob.miny));
	bounding_box.push_back(Point(blob.maxx, blob.maxy));
	bounding_box.push_back(Point(blob.minx, blob.maxy));
	bounding_box.push_back(Point(blob.minx, blob.miny));
	// Clear contour
	contour.clear();
	// Set contour
	contour = blobToPointVector(blob);
}

void drawBlobContour(Mat& frame, const cvb::CvBlob& blob, Scalar color)
{
	// Get chain code contour
	const cvb::CvChainCodes& chain_code = blob.contour.chainCode;
	// Get starting point
	Point current = blob.contour.startingPoint;
	for(cvb::CvChainCodes::const_iterator it=chain_code.begin(); it != chain_code.end(); it++)
	{
		// Draw point
		if(frame.channels() == 3)
		{
			Vec3b pixel(color[0], color[1], color[2]);
			frame.at<Vec3b>(current.y, current.x) = pixel;
		}
		else
		{
			//cout << current.y << " " << current.x << " " << frame.rows << " " << frame.cols << endl;
			frame.at<uchar>(current.y, current.x) = 255;
		}
		current.x += cvb::cvChainCodeMoves[*it][0];
		current.y += cvb::cvChainCodeMoves[*it][1];
	}
	// Get IplImage header, required by cvRenderContourChainCode
	//IplImage drawn_ipl = frame;
	// Draw contour
	//cvRenderContourChainCode(&(blob.contour), &drawn_ipl, color);
}

Mat contourBlob(cvb::CvBlob& blob)
{
	// Compute height and width of the minimum image size required (plus some margin) to contain the blob, in absolute frame coordinates 
	int width = blob.maxx + 10;
	int height = blob.maxy + 10;
	Mat drawn = Mat::zeros(height, width, CV_8UC3);
	Mat filtered = Mat::zeros(height, width, CV_8UC3);
	// Get IplImage header, required by cvRenderContourChainCode
	IplImage drawn_ipl = drawn;
	// Do some stuff which draws the contour
	cvRenderContourChainCode(&(blob.contour), &drawn_ipl, CV_RGB(255,255,255));
	//namedWindow("drawn");
	//imshow("drawn", drawn);
	//waitKey(0);
	cvtColor(drawn, filtered, CV_RGB2GRAY);
	vector<vector<Point> > contours;
	findContours(filtered, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	drawContours(drawn, contours, -1, CV_RGB(255,255,255), CV_FILLED, 8);
	cvtColor(drawn, filtered, CV_RGB2GRAY);
	Mat median = filtered;
	medianBlur(filtered, median, 3);
	// Crop the blob
	Rect box(blob.x, blob.y, blob.width(), blob.height());
	Mat final = median(box);
	//cvReleaseImage(&drawn_ipl);
	return final;
}

Mat rebuildBlobs(const cvb::CvBlobs& blobs, int width, int height)
{
	// Create drawing image, larger than the original frame, because cvRenderContourChainCode sucks
	Mat render = Mat::zeros(height + 20, width + 20, CV_8UC3);
	IplImage ipl_render = render;
	for (cvb::CvBlobs::const_iterator it=blobs.begin(); it!=blobs.end(); ++it)
	{
		cvb::CvBlob to_draw;
		cvb::cvCloneBlob(*(it->second), to_draw);
		to_draw.contour.startingPoint.x += 5;
		to_draw.contour.startingPoint.y += 5;
		//cvRenderContourChainCode(&it->second->contour, &ipl_render, CV_RGB(255, 255,255));
		cvRenderContourChainCode(&to_draw.contour, &ipl_render, CV_RGB(255, 255,255));
	}
	Mat render_c1;
	cvtColor(render, render_c1, CV_BGR2GRAY);
	vector<vector<Point> > contours;
       	findContours(render_c1, contours, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE);
	Mat filled = Mat::zeros(height + 20, width + 20, CV_8UC3);
	for(unsigned int i=0; i<contours.size(); i++)
	{
		drawContours(filled, contours, i, CV_RGB(255,255,255), -1);	
	}
	Mat output;
	cvtColor(filled, output, CV_BGR2GRAY);
	return output(Range(5, height+5), Range(5, width+5));
	//IplImage *drawn_frame = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
	//IplImage *filtered_image=cvCreateImage(cvSize(width, height),IPL_DEPTH_8U,1);
	//CvMemStorage* storage = cvCreateMemStorage(0);
	//CvSeq* contour = 0;
	//cvZero(drawn_frame);
	/*for (cvb::CvBlobs::const_iterator it=blobs.begin(); it!=blobs.end(); ++it)
		cvRenderContourChainCode(&it->second->contour, drawn_frame, CV_RGB(255, 255,255));
	cvCvtColor(drawn_frame, filtered_image, CV_RGB2GRAY);
	cvFindContours(filtered_image, storage, &contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );
	for( ; contour != 0; contour = contour->h_next )
		cvDrawContours( drawn_frame, contour, CV_RGB(255,255,255), CV_RGB(255,255,255 ), 0, -1, 8 );
	cvCvtColor(drawn_frame, filtered_image, CV_RGB2GRAY);
	cvSmooth(filtered_image, filtered_image, CV_MEDIAN, 3, 3);
	cvReleaseImage(&drawn_frame);
	cvReleaseMemStorage(&storage);
	return filtered_image;*/
}

IplImage *rebuildBlob(cvb::CvBlob& blob)
{
	int width = blob.maxx + 10;
	int height = blob.maxy + 10;
	IplImage *drawn_frame = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
	IplImage *filtered_image=cvCreateImage(cvSize(width, height),IPL_DEPTH_8U,1);
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* contour = 0;
	cvZero(drawn_frame);
	cvRenderContourChainCode(&(blob.contour), drawn_frame, CV_RGB(255, 255,255));
	cvCvtColor(drawn_frame, filtered_image, CV_RGB2GRAY);
	cvFindContours(filtered_image, storage, &contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );
	for( ; contour != 0; contour = contour->h_next )
		cvDrawContours( drawn_frame, contour, CV_RGB(255,255,255), CV_RGB(255,255,255 ), 0, -1, 8 );
	cvCvtColor(drawn_frame, filtered_image, CV_RGB2GRAY);
	cvSmooth(filtered_image, filtered_image, CV_MEDIAN, 3, 3);
	cvReleaseImage(&drawn_frame);
	cvReleaseMemStorage(&storage);
	return filtered_image;
}

// Co-occurrence matrix:
// - image: input image
// - position: offset between the two pixels to correlate
// - num_bins: size of output matrix
Mat computeCoOccurrenceMatrix(const Mat& image, Point position, int num_bins, bool normalize)
{
	// Define co-occurrence matrix
	Mat glcm = Mat::zeros(num_bins, num_bins, CV_32F);
	// Compute bin width
	float bin_width = 256.0f/num_bins;
	// Go through all pixels
	for(int i=0; i<image.rows; i++)
	{
		for(int j=0; j<image.cols; j++)
		{
			// Get "other pixel"'s coordinates
			int i2 = i + position.y;
			int j2 = j + position.x;
			// If out of bounds, skip
			if(i2 < 0 || j2 < 0 || i2 >= image.rows || j2 >= image.cols)
			{
				continue;
			}
			// Read pixel values
			float value1 = (float) image.at<uchar>(i,j);
			float value2 = (float) image.at<uchar>(i2,j2);
			// Compute corresponding bins
			int bin1 = floor(value1/bin_width);
			int bin2 = floor(value2/bin_width);
			// Increase occurrence count
			glcm.at<float>(bin2,bin1) = glcm.at<float>(bin2,bin1) + 1;
		}
	}
	// Normalize by sum of elements, if required
	if(normalize)
	{
		float total_sum = sum(glcm)[0];
		glcm = glcm / total_sum;
	}
	// Return matrix
	return glcm;
}
