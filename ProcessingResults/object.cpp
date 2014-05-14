// Represents a detected object.
// Polygons are represented as vector<Point> and are always internally stored such to have the last point equal to the initial point, even if the provided input vectors don't have this property.

#include "object.h"
#include <opencv_utils.h>
#include <highgui.h>

using namespace std;
using namespace cv;
		
pr::Object::Object(const pr::Object& copy)
{
	id = copy.id;
	frame_number = copy.frame_number;
	contour = copy.contour;
	detection_certainty = copy.detection_certainty;
	tracking_certainty = copy.tracking_certainty;
	type = copy.type;
	subtype = copy.subtype;
	optional = copy.optional;
}
		
// Assign method
pr::Object& pr::Object::operator=(const pr::Object& other)
{
	if(this == &other)
		return *this;
	id = other.id;
	frame_number = other.frame_number;
	contour = other.contour;
	detection_certainty = other.detection_certainty;
	tracking_certainty = other.tracking_certainty;
	type = other.type;
	subtype = other.subtype;
	optional = other.optional;
	return *this;
}

/*void pr::Object::setBoundingBox(const vector<Point>& b)
{
	// Check input vector
	if(b.size() < 4 || b.size() > 5 || (b.size() == 5 && b[0] != b[4]))
	{
		throw MyException("Bounding box must have 4 or 5 points (and if 5, the last one must be equal to the first one)");
	}
	// Copy vector
	bounding_box = b;
	// Check if we need to add the fifth point
	if(bounding_box.size() == 4)
	{
		// Add the first point
		bounding_box.push_back(bounding_box.front());
	}
}*/

void pr::Object::setContour(const std::vector<cv::Point>& c)
{
	// Check input vector
	if(c.size() < 3)
	{
		throw MyException("A valid contour must have at least 3 points");
	}
	// Copy vector
	contour = c;
	// Check if we need to add the last point
	if(c.front() != c.back())
	{
		// Add the first point
		contour.push_back(c.front());
	}
}
		
bool pr::Object::operator==(const pr::Object& other) const
{
	//showObject("compare_1", o1);
	//showObject("compare_2", o2);
	//waitKey(0);
	// Check frame number
	if(frame_number != other.frame_number)
	{
		return false;
	}
	// Check bounding box
	/*int min_x_1, max_x_1, min_y_1, max_y_1; 
	int min_x_2, max_x_2, min_y_2, max_y_2; 
	minMaxCoords(getBoundingBox(),       &min_x_1, &max_x_1, &min_y_1, &max_y_1);
	minMaxCoords(other.getBoundingBox(), &min_x_2, &max_x_2, &min_y_2, &max_y_2);
	if(min_x_1 != min_x_2 || max_x_1 != max_x_2 || min_y_1 != min_y_2 || max_y_1 != max_y_2)
	{
		return false;
	}*/
	// Check contour
	cvb::CvBlob* blob_1 = createBlob(getContour());	
	cvb::CvBlob* blob_2 = createBlob(other.getContour());	
	Mat mat_1 = blobToMat(*blob_1, CV_POSITION_ORIGINAL, false);
	Mat mat_2 = blobToMat(*blob_2, CV_POSITION_ORIGINAL, false);
	Mat compare = (mat_1 != mat_2);
	cvReleaseBlob(blob_1);
	cvReleaseBlob(blob_2);
	double min_val, max_val;
	minMaxLoc(compare, &min_val, &max_val);
	if(max_val > 0)
	{
		return false;
	}
	// Ok
	return true;
}
		
bool pr::Object::operator<(const Object& other) const
{
	if(frame_number < other.frame_number)
	{
		return true;
	}
	else if(frame_number > other.frame_number)
	{
		return false;
	}
	// Check bounding box
	vector<Point> this_bounding_box = boundingBoxFromContour(getContour());
	vector<Point> other_bounding_box = boundingBoxFromContour(other.getContour());
	int min_x_1, max_x_1, min_y_1, max_y_1; 
	int min_x_2, max_x_2, min_y_2, max_y_2; 
	minMaxCoords(this_bounding_box, &min_x_1, &max_x_1, &min_y_1, &max_y_1);
	minMaxCoords(other_bounding_box, &min_x_2, &max_x_2, &min_y_2, &max_y_2);
	// Check min y
	if(min_y_1 < min_y_2)
	{
		return true;
	}
	else if(min_y_1 > min_y_2)
	{
		return false;
	}
	else
	{
		// Same min y, check min x
		if(min_x_1 < min_x_2)
		{
			return true;
		}
		else if(min_x_1 > min_x_2)
		{
			return false;
		}
		else
		{
			// Same min x and y, check max y
			if(max_y_1 < max_y_2)
			{
				return true;
			}
			else if(max_y_1 > max_y_2)
			{
				return false;
			}
			else
			{
				// Finally, check max x
				if(max_x_1 < max_x_2)
				{
					return true;
				}
				else if(max_x_1 > max_x_2)
				{
					return false;
				}
			}
		}
	}
	// Check contour; in theory, both images show have the same size
	cvb::CvBlob* blob_1 = createBlob(this_bounding_box, getContour());	
	cvb::CvBlob* blob_2 = createBlob(other_bounding_box, other.getContour());	
	Mat mat_1 = blobToMat(*blob_1, CV_POSITION_ORIGINAL, false);
	Mat mat_2 = blobToMat(*blob_2, CV_POSITION_ORIGINAL, false);
	//Mat compare = (mat_1 < mat_2);
	// Check each pixel, the first one which has a zero, is the lesser
	for(int i=0; i<mat_1.rows; i++)
	{
		for(int j=0; j<mat_1.cols; j++)
		{
			if(mat_1.at<uchar>(i,j) < mat_2.at<uchar>(i,j))
			{
				return true;
			}
			else if(mat_1.at<uchar>(i,j) > mat_2.at<uchar>(i,j))
			{
				return false;
			}
		}
	}
	return false;
}
