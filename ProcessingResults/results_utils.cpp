#include "results_utils.h"
#include <opencv_utils.h>
#include <log.h>

using namespace cv;
using namespace std;

namespace pr
{
		
	// Filter by removing objects touching the border
	Results ResultsUtils::filterByBorder(const Results& results, int height, int width)
	{
		// Initialize new results
		Results new_results;
		// Get id list
		list<int> ids = results.idList();
		// Go through each id
		for(list<int>::iterator id_it = ids.begin(); id_it != ids.end(); ++id_it)
		{
			// Get object list
			ObjectList objects = results.getObjectsById(*id_it);
			// Go through each detection
			for(ObjectList::iterator o_it = objects.begin(); o_it != objects.end(); o_it++)
			{
				try
				{
					// Get object
					Object& object = *o_it;
					// Get bounding box
					vector<Point> bounding_box = boundingBoxFromContour(object.getContour());
					// Check points
					for(vector<Point>::iterator p_it = bounding_box.begin(); p_it != bounding_box.end(); p_it++)
					{
						if(p_it->x <= 0 ||
						   p_it->x >= width-1 ||
						   p_it->y <= 0 ||
						   p_it->y >= height-1)
						{
							// Don't add and go to next object
							continue;
						}
					}
					// If we get here, add object
					new_results.addObject(object);
				}
				catch(MyException& e)
				{
					Log::w() << "Error: " << e.what() << endl;
				}
			}
		}
		// Return new results
		return new_results;
	}
		
	// Draw foreground mask for given frame
	cv::Mat ResultsUtils::drawMask(const Results& results, int frame_number, int height, int width)
	{
		// Get objects in this frame
		ObjectList objects = results.getObjectsByFrame(frame_number);
		// If height and width are not provided, infer them from input objects
		if(width < 0 || height < 0)
		{
			for(ObjectList::const_iterator o_it = objects.begin(); o_it != objects.end(); o_it++)
			{
				// Get object
				const Object& object = *o_it;
				// Get bounding box
				vector<Point> bounding_box = boundingBoxFromContour(object.getContour());
				// Check each point
				for(vector<Point>::iterator p_it = bounding_box.begin(); p_it != bounding_box.end(); p_it++)
				{
					if(p_it->x >= width)	width = p_it->x + 1;
					if(p_it->y >= height)	height = p_it->y + 1;
				}
			}
		}
		// Initialize output image
		Mat mask = Mat::zeros(height, width, CV_8UC1);
		// Draw each object's mask
		for(ObjectList::const_iterator o_it = objects.begin(); o_it != objects.end(); o_it++)
		{
			::drawContour(o_it->getContour(), mask, CV_RGB(0,255,250), CV_FILLED);
		}
		// Return mask
		return mask;
	}
		
	// Get color associated to object ID
	Scalar ResultsUtils::objectColor(int id)
	{
		// Initialize seed
		srand(id);
		// Get R component
		int r = 255*((float)rand())/((float)RAND_MAX);
		// Get G component so that it is at least 50 values far from R
		int g;
		do
		{
			g = 255*((float)rand())/((float)RAND_MAX);
		} while(abs(r-g) < 70);
		// Get B component so that it is at least 50 values far from R and G
		int b;
		do
		{
			b = 255*((float)rand())/((float)RAND_MAX);
		} while(abs(b-r) < 70 && abs(b-g) < 70);
		// Return color
		return CV_RGB(r,g,b);
	}

	// Filter by average size per object
	Results ResultsUtils::filterByAverageSize(const Results& results, int threshold)
	{
		// Initialize new results
		Results new_results;
		// Get id list
		list<int> ids = results.idList();
		// Go through each id
		for(list<int>::iterator id_it = ids.begin(); id_it != ids.end(); ++id_it)
		{
			// Get object list
			ObjectList objects = results.getObjectsById(*id_it);
			// Initialize average certainty and count of valid values
			float avg_size = 0.0f;
			// Go through each detection
			for(ObjectList::iterator o_it = objects.begin(); o_it != objects.end(); o_it++)
			{
				try
				{
					// Get object
					Object& object = *o_it;
					// Convert to blob
					cvb::CvBlob* blob = createBlob(object.getContour());
					// Add area
					avg_size += blob->area;
					// Free blob
					cvReleaseBlob(blob);
				}
				catch(MyException& e)
				{
					Log::w() << "Error: " << e.what() << endl;
				}
			}
			// Perform average
			avg_size /= objects.size();
			Log::d() << "Object id " << *id_it << ", avg size: " << avg_size << ", ";
			// Check threshold
			if(avg_size >= threshold)
			{
				// Add to output results
				Log::d() << "adding." << endl;
				new_results.addObjects(objects);
			}
			else
			{
				Log::d() << "skipping." << endl;
			}
		}
		// Return new results
		return new_results;
	}

	// Filter by size
	Results ResultsUtils::filterBySize(const Results& results, int threshold)
	{
		// Initialize new results
		Results new_results;
		// Get id list
		list<int> ids = results.idList();
		// Go through each id
		for(list<int>::iterator id_it = ids.begin(); id_it != ids.end(); ++id_it)
		{
			// Get object list
			ObjectList objects = results.getObjectsById(*id_it);
			// Go through each detection
			for(ObjectList::iterator o_it = objects.begin(); o_it != objects.end(); o_it++)
			{
				try
				{
					// Get object
					Object& object = *o_it;
					// Convert to blob
					cvb::CvBlob* blob = createBlob(object.getContour());
					// Check area
					Log::d() << "Blob area: " << blob->area << ", ";
					if((int)blob->area >= threshold)
					{
						// Add object
						new_results.addObject(object);
						Log::d() << "adding." << endl;
					}
					else
						Log::d() << "skipping." << endl;
					// Free blob
					cvReleaseBlob(blob);
				}
				catch(MyException& e)
				{
					Log::w() << "Error: " << e.what() << endl;
				}
			}
		}
		// Return new results
		return new_results;
	}

	// Filter by number of appearances
	Results ResultsUtils::filterByNumAppearances(const Results& results, int threshold, int upper_threshold)
	{
		// Initialize new results
		Results new_results;
		// Get id list
		list<int> ids = results.idList();
		// Go through each id
		for(list<int>::iterator id_it = ids.begin(); id_it != ids.end(); ++id_it)
		{
			// Get object list
			ObjectList objects = results.getObjectsById(*id_it);
			// Check number of appearances
			if(objects.size() >= threshold && objects.size() <= upper_threshold)
			{
				Log::d() << "Adding object " << *id_it << ", " << objects.size() << " detections." << endl;
				// Add to output results
				new_results.addObjects(objects);
			}
			else
			{
				Log::d() << "Skipping object " << *id_it << " with " << objects.size() << " detections." << endl;
			}
		}
		// Return new results
		return new_results;
	}
	
	// Filter by size
	Results ResultsUtils::filterByDetectionCertainty(const Results& results, float threshold, bool add_by_default)
	{
		Log::d() << "Certainty threshold: " << threshold << endl;
		// Initialize new results
		Results new_results;
		// Get id list
		list<int> ids = results.idList();
		// Go through each id
		for(list<int>::iterator id_it = ids.begin(); id_it != ids.end(); ++id_it)
		{
			// Get object list
			ObjectList objects = results.getObjectsById(*id_it);
			// Go through each detection
			for(ObjectList::iterator o_it = objects.begin(); o_it != objects.end(); o_it++)
			{
				// Get object
				Object& object = *o_it;
				// Check invalid certainty
				if(object.getDetectionCertainty() < 0 && add_by_default)
				{
					// Add object
					Log::d() << "Object " << *id_it << " has invalid certainty (" << object.getDetectionCertainty() << "), adding by default." << endl;
					new_results.addObject(object);
					continue;
				}
				// Compare to threshold
				if(object.getDetectionCertainty() >= threshold)
				{
					// Add object
					Log::d() << "Object " << *id_it << ", certainty: " << object.getDetectionCertainty() << " (threshold: " << threshold << "), added." << endl;
					new_results.addObject(object);
				}
				else
				{
					Log::d() << "Object " << *id_it << ", certainty: " << object.getDetectionCertainty() << " (threshold: " << threshold << "), skipped." << endl;
				}
			}
		}
		// Return new results
		return new_results;
	}

	// Filter by average detection certainty per object
	Results ResultsUtils::filterByAverageDetectionCertainty(const Results& results, float threshold, bool add_by_default)
	{
		// Initialize new results
		Results new_results;
		// Get id list
		list<int> ids = results.idList();
		// Go through each id
		for(list<int>::iterator id_it = ids.begin(); id_it != ids.end(); ++id_it)
		{
			// Get object list
			ObjectList objects = results.getObjectsById(*id_it);
			// Initialize average certainty and count of valid values
			int valid_certainties = 0;
			float avg_certainty = 0.0f;
			// Go through each detection
			for(ObjectList::iterator o_it = objects.begin(); o_it != objects.end(); o_it++)
			{
				// Get certainty
				float certainty = o_it->getDetectionCertainty();
				// Check
				if(certainty > -1)
				{
					valid_certainties++;
					avg_certainty += certainty;
				}
			}
			// Check number of valid certainties
			if(valid_certainties == 0)
			{
				if(!add_by_default)
				{
					// Skip object
					Log::d() << "Object " << *id_it << " has no valid certainties, skipping." << endl;
					continue;
				}
				else
				{
					// Add to output results
					Log::d() << "Object " << *id_it << " has no valid certainties, adding by default." << endl;
					new_results.addObjects(objects);
				}
			}
			else
			{
				// Perform average
				avg_certainty /= valid_certainties;
				Log::d() << "Object id " << *id_it << ": " << avg_certainty << ", ";
				// Check threshold
				if(avg_certainty >= threshold)
				{
					// Add to output results
					Log::d() << "adding." << endl;
					new_results.addObjects(objects);
				}
				else
				{
					Log::d() << "skipping." << endl;
				}
			}
		}
		// Return new results
		return new_results;
	}

	// Draw results on frame
	void ResultsUtils::drawObjects(const Results& results, cv::Mat& frame, int frame_number, cv::Scalar color, int shadow_time, int thickness)
	{
		// Find all objects to be drawn (TODO and in the shadow time interval)
		ObjectList frame_objects = results.getObjectsByFrame(frame_number);
		//Log::debug(0) << "Found " << frame_objects.size() << endl;
		// Go through each object
		for(ObjectList::iterator o_it = frame_objects.begin(); o_it != frame_objects.end(); ++o_it)
		{
			// Get all appearances of this object
			ObjectList detections = results.getObjectsById(o_it->getId());
			//Log::debug(0) << "Found " << detections.size() << " detections for object " << o_it->getId() << endl;
			// Set object color
			if(color[0] == -1 || color[1] == -1 || color[2] == -1)
			{
				color = objectColor(o_it->getId());
			}
			// Get iterator to first element
			ObjectList::iterator d_it = detections.begin();
			// Check if we have to draw the contour now (i.e., if we only have one detection)
			if(d_it->getFrameNumber() == frame_number)
			{
				// Draw contour
				//Log::debug(0) << "Drawing first contour" << endl;
				vector<vector<Point> > contour_container;
				contour_container.push_back(d_it->getContour());
				drawContours(frame, contour_container, -1, color, thickness);
			}
			// Get starting point
			Point start = center(boundingBoxFromContour(d_it->getContour()));
			// Draw track
			for(++d_it; d_it != detections.end(); ++d_it)
			{
				//Log::debug(0) << "Detection frame: " << d_it->getFrameNumber() << endl;
				// Check frame number
				if(d_it->getFrameNumber() > frame_number)
				{
					//Log::debug(0) << "Beyond current frame." << endl;
					break;
				}
				else if(d_it->getFrameNumber() == frame_number)
				{
					// Draw contour
					//Log::debug(0) << "Drawing contour" << endl;
					vector<vector<Point> > contour_container;
					contour_container.push_back(d_it->getContour());
					drawContours(frame, contour_container, -1, color, thickness);
				}
				// Get current position
				Point end = center(boundingBoxFromContour(d_it->getContour()));
				// Draw line
				//Log::debug(0) << "Drawing line from " << start << " to " << end << endl;
				//line(frame, start, end, color, 2);
				// Update start
				start = end;
			}
		}
	}
	
	// Draw all trajectories
	void ResultsUtils::drawTrajectories(const Results& results, cv::Mat& frame)
	{
		// Get all object IDs
		list<int> ids = results.idList();
		// Go through each object
		for(list<int>::iterator id_it = ids.begin(); id_it != ids.end(); id_it++)
		{
			//Log::d() << "Object id: " << *id_it << endl;
			// Get all appearances of this object
			ObjectList detections = results.getObjectsById(*id_it);
			// Set object color
			Scalar color = objectColor(detections.begin()->getId());
			// Get iterator to first element
			ObjectList::iterator d_it = detections.begin();
			// Get starting point
			Point start = center(boundingBoxFromContour(d_it->getContour()));
			//Log::d() << "point: " << start.x << ", " << start.y << endl;
			// Draw track
			for(++d_it; d_it != detections.end(); ++d_it)
			{
				// Get current position
				Point end = center(boundingBoxFromContour(d_it->getContour()));
				//Log::d() << "point: " << end.x << ", " << end.y << endl;
				// Draw line
				line(frame, start, end, color, 2);
				// Update start
				start = end;
			}
		}
	}
		
	// Draw methods for single object/trajectory
	void ResultsUtils::drawBoundingBox(const Object& object, Mat& frame, int thickness, Scalar color)
	{
		// Set color
		if(color[0] < 0 || color[1] < 0 || color[2] < 0)
		{
			// Set color according to object ID
			color = objectColor(object.getId());
		}
		// Get minimum and maximum contour coordinates
		int min_x, min_y, max_x, max_y;
		minMaxCoords(object.getContour(), &min_x, &max_x, &min_y, &max_y);
		// Draw bounding box
		rectangle(frame, Point(min_x, min_y), Point(max_x, max_y), color, thickness);
	}

	void ResultsUtils::drawContour(const Object& object, Mat& frame, int thickness, Scalar color)
	{
		// Set color
		if(color[0] < 0 || color[1] < 0 || color[2] < 0)
		{
			// Set actual color according to object ID
			color = objectColor(object.getId());
		}
		// Draw contour
		::drawContour(object.getContour(), frame, color, thickness);
	}

	void ResultsUtils::drawTrajectory(const ObjectList& objects, Mat& frame, int from_frame, int to_frame, int thickness, Scalar color)
	{
		// Check size
		if(objects.size() < 2)
		{
			return;
		}
		// Set color
		if(color[0] < 0 || color[1] < 0 || color[2] < 0)
		{
			// Set actual color according to object ID
			color = objectColor(objects.begin()->getId());
		}
		// Initialize point list
		vector<Point> points;
		// Go through all objects
		for(ObjectList::const_iterator o_it = objects.begin(); o_it != objects.end(); o_it++)
		{
			// Check frame
			int f = o_it->getFrameNumber();
			if((from_frame >= 0 && f < from_frame) || (to_frame >= 0 && f > to_frame))
			{
				// Skip object
				continue;
			}
			// Get objects centroid
			Point center = centroid(o_it->getContour());
			// Add to list
			points.push_back(center);
		}
		// Draw point list
		drawMultiLine(points, frame, color, thickness);
	}
		
	void ResultsUtils::drawSpecies(const Object& object, Mat& frame, Scalar color)
	{
		// Set color
		if(color[0] < 0 || color[1] < 0 || color[2] < 0)
		{
			// Set actual color according to object ID
			color = objectColor(object.getId());
		}
		// Compute text size
		int baseline;
		Size size = getTextSize(object.getSubType(), FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseline);
		// Get object bounding box
		int min_x, min_y, max_x, max_y;
		minMaxCoords(object.getContour(), &min_x, &max_x, &min_y, &max_y);
		// Compute the space left on the top and on the bottom
		int top_space = min_y - 3;
		int bottom_space = frame.rows - max_y - 3;
		// Check where we want to put the label
		Point origin;
		origin.x = (min_x + max_x)/2 - size.width/2;
		if(top_space >= size.height || bottom_space < size.height)
		{
			// Put writing on the top
			origin.y = min_y - 3;
		}
		else
		{
			// Put writing on the bottom
			origin.y = max_y + 3 + size.height;
		}
		// Draw label
		putText(frame, object.getSubType(), origin, FONT_HERSHEY_SIMPLEX, 0.5, color, 1);
	}
}
