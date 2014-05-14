// Class for covariance tracking.
//  Naming conventions:
// - W: image width
// - H: image height
// - d: number of features

#include "covariance_tracker.h"
#include <math_utils.h>
#include <shell_utils.h>
#include <opencv_utils.h>
#include <list>

// Debug includes
#include <iostream>
#include <highgui.h>
#include <sstream>

using namespace std;

namespace alg
{
	// Default constructor
	CovarianceTracker::CovarianceTracker() : drawSearchAreaFlag(true)
	{
		// Configuration settings for 320x480@5/8fps
		parameters.add<int>("objectTTL", 6);
		parameters.add<int>("searchAreaIncrementForObject", 4);
		parameters.add<int>("searchAreaIncrementForDirection", 12);
		parameters.add<int>("trackingMemory", 10);
		parameters.add<float>("maxCovarianceDistance", 2);
		parameters.add<bool>("enableCovarianceDistanceCheck", false);

		parameters.add<bool>("enable_xy", true);
		parameters.add<bool>("enable_rgb", true);
		parameters.add<bool>("enable_hue", true);
		parameters.add<bool>("enable_hist", false);
		parameters.add<int>("search_area_size_quantization", 400);
		parameters.add<float>("search_area_object_weight", 0.5);
		parameters.add<float>("search_area_increment", 0.1);
	}
		
	// Quantize area
	int CovarianceTracker::quantizeArea(int area)
	{
		int q = parameters.get<int>("search_area_size_quantization");
		int q_level = area/q + 1;
		int q_area = q*q_level;
		return q_area;
	}
	
	// Compute search area for tracked object
	int CovarianceTracker::computeSearchArea(const CovarianceTrackedObject& to)
	{
		// Initialize variables
		int average_search_area = -1;
		int object_search_area = -1;
		bool average_area_valid = false;
		bool object_area_valid = false;
		// Compute quantized area
		int area = quantizeArea(to.currentRegion().area);
		// Check average area
		if(num_samples_per_size.find(area) != num_samples_per_size.end())
		{
			//Log::debug() << "Found " << num_samples_per_size[area] << " samples for area " << area << endl;
			average_area_valid = true;
			average_search_area = sum_distance_per_size[area]/num_samples_per_size[area];
		}
		else
		{
			//Log::debug() << "No samples found for area " << area << endl;
		}
		// Check object average movement
		object_search_area = to.getAverageMovingDistance();
		//Log::debug() << "Object average moving distance: " << object_search_area << endl;
		if(object_search_area > 0)
		{
			object_area_valid = true;
		}
		// Initialize search area with object's dimensions search area
		int max_dimension = to.currentRegion().width() > to.currentRegion().height() ? to.currentRegion().width() : to.currentRegion().height();
		int search_area = max_dimension;
		if(average_area_valid && !object_area_valid)
		{
			search_area = average_search_area;// + max_dimension/2;
			Log::debug() << "Using only average search area for size " << area << ": " << search_area << endl;
		}
		else if(!average_area_valid && object_area_valid)
		{
			search_area = object_search_area;// + max_dimension/2;
			Log::debug() << "Using only object's average movement as search area for size " << area << ": " << search_area << endl;
		}
		else if(average_area_valid && object_area_valid)
		{
			float alpha = parameters.get<float>("search_area_object_weight");
			search_area = (1.0f - alpha)*average_search_area + alpha*object_search_area;// + max_dimension/2;
			Log::debug() << "Using average search area (" << average_search_area << ") and object's average movement (" << object_search_area << ") as search area for size " << area << ": " << search_area << endl;
		}
		else
		{
			Log::debug() << "Using object's max dimension as search area for size " << area << ": " << search_area << endl;
		}
		// Return result; if it's 0, return 5
		if(search_area == 0)
		{
			return 5;
		}
		return search_area;
	}


	// Destructore; has to free tracked objects
	CovarianceTracker::~CovarianceTracker()
	{
		// Free currently tracked objects
		for(vector<TrackedObject*>::iterator it = tracked_objects.begin(); it != tracked_objects.end(); it++)
		{
			delete (CovarianceTrackedObject*) *it;
		}
		// Free previously tracked objects
		for(vector<TrackedObject*>::iterator it = lost_objects.begin(); it != lost_objects.end(); it++)
		{
			delete (CovarianceTrackedObject*) *it;
		}
	}
		
	// Find object (i.e. square region containing object) in given area
	Rect CovarianceTracker::findObject(const Mat& model, const Mat& frame, const Rect& area, int min_size, int max_size, int step) const
	{
		// Get ROI
		Mat region = frame(area).clone();
		// Define minimum distance and corresponding region
		float min_dist = INT_MAX;
		Rect min_rect(-1, -1, -1, -1);
		// Go through all sizes
		for(int size=min_size; size<=max_size; size++)
		{
			// Check size is smaller than minimum dimension of search area
			if(size > area.width || size > area.height)
			{
				// Stop search
				break;
			}
			// Go through all windows
			for(int i=0; i<=area.height-size; i += step)
			{
				for(int j=0; j<=area.width-size; j += step)
				{
					try
					{
						// Compute rectangle
						Rect window(j, i, size, size);
						// Get ROI
						Mat region_window = region(window);
						// Compute covariance matrix over window
						Mat feature_matrix = featureExtractor->computeFeatureSet(region_window);
						Mat covar_mat = MathUtils::computeCovarianceMatrix(feature_matrix);
						// Compute distance from model
						float covar_distance = MathUtils::covarianceDistance(model, covar_mat);
						//Log::d() << "Region [" << j << " " << i << " " << size << " " << size << "]: " << covar_distance << endl;
						// Check if minimum distance
						if(covar_distance < min_dist)
						{
							min_dist = covar_distance;
							min_rect = window;
						}
					}
					catch(MyException& e)
					{
						Log::warning() << "Error in the computation of the covariance distance for rect [" << area.x << " " << area.y << " " << area.width << " " << area.height << "]: " << e.what() << endl;
					}
					catch(...)
					{
						Log::warning() << "Error in the computation of the covariance distance for rect [" << area.x << " " << area.y << " " << area.width << " " << area.height << "]." << endl;
					}
				}
			}
		}
		// If the result is ok, move it back to the original region
		if(min_rect.height > 0 && min_rect.width > 0)
		{
			min_rect += Point(area.x, area.y);
		}
		Log::debug() << "Minimum distance: " << min_dist << endl;
		// Return result
		return min_rect;
	}

	// Get search regions
	void CovarianceTracker::computeSearchRegions(const CovarianceTrackedObject& tracked_object, Rect& t_rect, Rect& dir_rect)
	{
		// Compute tracked object's search regions
		int search_area = computeSearchArea(tracked_object);
		//Log::debug() << "Search area: " << search_area << endl;
		// Get the region occupied by the tracked object
		const cvb::CvBlob& t_region = tracked_object.currentRegion();
		//Log::debug() << "Blob: [" << t_region.x << ", " << t_region.y << ", " << t_region.width() << ", " << t_region.height() << "]" << endl;
		// Compute the center of current object
		Point center((t_region.x + t_region.maxx)/2, (t_region.y+t_region.maxy)/2);
		// Extend it if the object has been missing
		int expand_factor = parameters.get<int>("objectTTL") - tracked_object.TTL();
		Log::debug() << "Expand factor: " << expand_factor << endl;
		int expand_size = expand_factor*parameters.get<float>("search_area_increment")*search_area;
		search_area += expand_size;
		// Initialize central region
		t_rect = Rect(center.x - search_area, center.y - search_area, 2*search_area, 2*search_area);
		// Initialize direction region
		dir_rect = Rect(0, 0, 0, 0);
		// Check we have a previous detection, to compute the direction
		if(tracked_object.frameList().size() > 1)
		{
			// Search area size
			dir_rect.height = 2*search_area;
			dir_rect.width = 2*search_area;
			// Compute the center of the previous detection
			const cvb::CvBlob& prev_region = *(tracked_object.regionList().rbegin() + 1);
			Point prev_center((prev_region.x + prev_region.maxx)/2, (prev_region.y+prev_region.maxy)/2);
			// Compute the horizontal/vertical distance between the two centers
			int delta_center = search_area/2;
			// Update the center
			dir_rect.x = center.x - search_area + delta_center*(center.x > prev_center.x ? 1 : -1);
			dir_rect.y = center.y - search_area + delta_center*(center.y > prev_center.y ? 1 : -1);
		}
	}
		
	// Read next frame, frame number, object blogs binary masks and perform tracking.
	// The input image type is supposed to be CV_8UC3.
	void CovarianceTracker::nextFrame(const Mat& frame, int frame_number, const cvb::CvBlobs& blobs)
	{
		// Feature selection at initialization
		static bool first_call = true;
		if(first_call)
		{
			// Setup feature extractors
			FeatureExtractorSet *covariance_feature_extractor = new FeatureExtractorSet();
			if(parameters.get<bool>("enable_xy"))		covariance_feature_extractor->addExtractor(new XYFeatureExtractor());
                        if(parameters.get<bool>("enable_rgb"))		covariance_feature_extractor->addExtractor(new RGBFeatureExtractor());
                        if(parameters.get<bool>("enable_hue"))		covariance_feature_extractor->addExtractor(new HueFeatureExtractor());
                        if(parameters.get<bool>("enable_hist"))		covariance_feature_extractor->addExtractor(new HistFeatureExtractor());
                       	featureExtractor = covariance_feature_extractor;
			// Check feature number
			if(covariance_feature_extractor->featureNumber() == 0)
			{
				throw MyException("No features selected for tracker.");
			}
			// Reset flag
			first_call = false;
		}
		// Copy parameters to local variables
		int objectTTL = parameters.get<int>("objectTTL");
		int trackingMemory = parameters.get<int>("trackingMemory");
		float maxCovarianceDistance = parameters.get<int>("maxCovarianceDistance");
		bool enableCovarianceDistanceCheck = parameters.get<bool>("enableCovarianceDistanceCheck");
		// Debug (for writing image files)
		//static int i = 0;
		// Define list of detected objects
		vector<CovarianceDetectedObject> detected_objects;
		// Compute covariance matrices
		for(cvb::CvBlobs::const_iterator it=blobs.begin(); it!=blobs.end(); it++)
		{
			try
			{
				// Select blob
				cvb::CvBlob& blob = *(it->second);
				Rect bounding_box(blob.x, blob.y, blob.width(), blob.height());
				// Select object region
				Mat object(frame, bounding_box);
				// Compute object's feature matrix
				Mat object_mask = blobToMat(blob, CV_POSITION_TOP_LEFT);
				Mat feature_matrix = featureExtractor->computeFeatureSet(object, object_mask);
				//cout << "Feature matrix: " << endl << feature_matrix << endl;
				// Compute covariance matrix
				Mat covar_mat = MathUtils::computeCovarianceMatrix(feature_matrix);
				//cout << "Region covariance matrix: " << endl << covar_mat << endl << endl;
				// Define a new detected_object and add it to the list
				cvb::CvBlob copy;
				cvb::cvCloneBlob(blob, copy);
				CovarianceDetectedObject detected_object(covar_mat, copy);
				detected_objects.push_back(detected_object);
			}
			catch(MyException& e)
			{
				Log::warning() << ShellUtils::RED << "Error while creating the detected object: " << e.what() << ShellUtils::NORMAL << endl;
			}
			catch(...)
			{
				Log::warning() << ShellUtils::RED << "Error while creating the detected object." << ShellUtils::NORMAL << endl;
			}
		}
		// Build the list of all reasonable tracked_object-detected_object couples, with distances
		list<TDCandidate> candidates;
		// Go through all tracked objects
		if(tracked_objects.size() == 0)
		{
			Log::info() << "No objects tracked." << endl;
		}
		for(vector<TrackedObject*>::const_iterator t_it = tracked_objects.begin(); t_it != tracked_objects.end(); t_it++)
		{
			// Get reference to tracked object
			CovarianceTrackedObject& tracked_object = * ((CovarianceTrackedObject*) (*t_it));
			// Get search regions
			Rect t_rect, dir_rect;
			computeSearchRegions(tracked_object, t_rect, dir_rect);
			Log::debug() << "Tracked object search region: [" << t_rect.x << ", " << t_rect.y << ", " << t_rect.width << ", " << t_rect.height << "]" << endl;
			// Go through all detected objects
			for(vector<CovarianceDetectedObject>::iterator d_it = detected_objects.begin(); d_it != detected_objects.end(); d_it++)
			{
				CovarianceDetectedObject& detected_object = *d_it;
				// Get the region occupied by the detected object
				cvb::CvBlob d_region = detected_object.region();
				// Check if the tracked object and the detected object overlap
				Rect d_rect(d_region.x, d_region.y, d_region.width(), d_region.height());
				Rect overlap = (t_rect & d_rect);// | (dir_rect & d_rect);
				Log::debug() << "Checking overlap between tracked object [" << t_rect.x << ", " << t_rect.y << ", " << t_rect.width << ", " << t_rect.height << "] and detected object [" << d_rect.x << ", " << d_rect.y << ", " << d_rect.width << ", " << d_rect.height << "]: [" << overlap.x << ", " << overlap.y << ", " << overlap.width << ", " << overlap.height << "]" << endl;
				// Check if it is a feasible couple
				if(overlap.height > 0 && overlap.width > 0)
				{
					// Create the couple object
					TDPair couple(&tracked_object, &detected_object);
					// Compute the covariance distance between the tracked object and the region
					//Log::info() << "Calcolo covariance distance tra: " << tracked_object.covarianceModel() << endl << detected_object.covarianceMatrix() << endl;;
					float covar_distance = MathUtils::covarianceDistance(tracked_object.covarianceModel(), detected_object.covarianceMatrix());
					// Check if the covariance distance between the tracked object and the detected object is low enough
					if(enableCovarianceDistanceCheck && covar_distance > maxCovarianceDistance)
					{
						Log::info() << "Covariance distance too high (" << covar_distance << ") for object " << tracked_object.id << ", skipping." << endl;
						continue;
					}
					// Create the candidate object
					TDCandidate candidate(couple, covar_distance);
					// Add it to the candidate list
					candidates.push_back(candidate);
					//cout << "Object's covariance matrix: " << endl << tracked_object.covarianceModel() << endl << endl;
					Log::info() << "New candidate for tracked object " << tracked_object.id << " with covariance distance " << covar_distance << endl;
				}
				// Debug output
				else
				{
					Log::info() << "No overlap between detected object and object " << tracked_object.id << ", skipping." << endl;
				}
			}
		}
		// Sort candidates according to the distance
		candidates.sort(CovarianceTracker::compareCandidates);
		// Go through all candidates
		for(list<TDCandidate>::const_iterator it = candidates.begin(); it != candidates.end(); it++)
		{
			// Get tracked object and detection object references
			CovarianceTrackedObject& tracked_object = *((*it).first.first);
			CovarianceDetectedObject& detected_object = *((*it).first.second);
			// Check if the detected region has already been assigned to another object
			if(detected_object.isAssigned())
			{
				// Skip to next candidate
				continue;
			}
			// Check if we already have a detection for this object in this frame
			if(tracked_object.lastAppearance() == frame_number)
			{
				// Skip to next candidate
				continue;
			}
			// Assign the detected object to the tracked object.
			try
			{
				Log::info() << "Assigning detection to tracked object " << tracked_object.id << endl;
				tracked_object.addDetection(frame_number, detected_object.region(), detected_object.covarianceMatrix());
				// Update average moving distance by area
				if(tracked_object.lastMovementIsConsecutive())
				{
					// Get object area
					int area = tracked_object.currentRegion().area;
					Log::debug() << "Updating search area for area " << area << endl;
					// Quantize area
					area = quantizeArea(area);
					// Check if area exists
					if(num_samples_per_size.find(area) == num_samples_per_size.end())
					{
						num_samples_per_size[area] = 1;
						sum_distance_per_size[area] = tracked_object.getLastMovingDistance();
					}
					else
					{
						++num_samples_per_size[area];
						sum_distance_per_size[area] += tracked_object.getLastMovingDistance();
					}
				}
				// Notify the detected object that it has already been assigned (unassigned objects will be considered as new objects in the next stage)
				detected_object.notifyAssigned();
			}
			catch(...)
			{
				Log::warning() << "Error while adding the detection to this object, skipping." << endl;
			}
		}
		// Create a copy of the list of currently tracked object, and clear the original list (we'll add objects later)
		vector<TrackedObject*> tracked_objects_backup(tracked_objects);
		tracked_objects.clear();
		// Go through all currently existing tracked objects and check if they have been found in the current frame; if not, decrease the object's TTL
		for(vector<TrackedObject*>::iterator it = tracked_objects_backup.begin(); it != tracked_objects_backup.end(); it++)
		{
			// Get reference to object
			CovarianceTrackedObject& tracked_object = *((CovarianceTrackedObject*) (*it));
			// Check if the object has been found in the current frame
			if(tracked_object.lastAppearance() != frame_number)
			{
				Log::info() << "Tracked object " << tracked_object.id << " not found, decreasing TTL (now: " << (tracked_object.TTL()-1) << ")." << endl;
				// Decrease the TTL and check if it is 0
				if(tracked_object.decreaseTTL())
				{
					// Stop tracking this object (i.e. free its memory)
					tracked_object.stopTracking();
					// Add it to the lost object list
					lost_objects.push_back(&tracked_object);
					Log::info() << "Lost tracked object " << tracked_object.id << endl;
				}
				else
				{
					// Add the object to the tracked object list anyway
					tracked_objects.push_back(&tracked_object);
					Log::info() << "Re-adding tracked object " << tracked_object.id << " to the list" << endl;
				}
			}
			else
			{
				// Add the object to the list
				tracked_objects.push_back(&tracked_object);
				Log::info() << "Tracked object " << tracked_object.id << " found, adding it to the list" << endl;
			}
		}
		// Go through all unassigned detected objects and create new tracked objects from them
		for(vector<CovarianceDetectedObject>::iterator it = detected_objects.begin(); it != detected_objects.end(); it++)
		{
			// Get detected object reference
			CovarianceDetectedObject& detected_object = *it;
			// Check if the object has been assigned
			if(! detected_object.isAssigned())
			{
				// Create new tracked object instance
				CovarianceTrackedObject *tracked_object = new CovarianceTrackedObject(trackingMemory, objectTTL);
				Log::info() << "New tracked object, id: " << tracked_object->id << ", TTL: " << objectTTL << endl;
				try
				{
					// Add the current detection
					//cout << detected_object.covarianceMatrix() << endl;
					tracked_object->addDetection(frame_number, detected_object.region(), detected_object.covarianceMatrix());
					// Add the object to the tracked object list
					tracked_objects.push_back(tracked_object);
				}
				catch(MyException& e)
				{
					// Discard object
					Log::error() << ShellUtils::RED << "Error while initializing a new tracked object, discarding..." << endl;
					Log::error() << e.what() << ShellUtils::NORMAL << endl;
					delete tracked_object;
				}
			}
		}
		// I guess we're done here...
		//Log::info() << "Tracked objects: ["; for(int i=0; i<tracked_objects.size(); i++) Log::info(0) << tracked_objects[i]->id << " "; Log::info(0) << "]" << endl;
		//Log::info() << "Lost objects: ["; for(int i=0; i<lost_objects.size(); i++) Log::info(0) << lost_objects[i]->id << " "; Log::info(0) << "]" << endl;
	}
		
	// Adapt tracking parameters to video properties
	void CovarianceTracker::adaptToVideo(VideoCapture& video_cap)
	{
		// Read video properties
		float frame_rate = video_cap.get(CV_CAP_PROP_FPS);
		//float frame_height = video_cap.get(CV_CAP_PROP_FRAME_HEIGHT);
		float frame_width = video_cap.get(CV_CAP_PROP_FRAME_WIDTH);
		// Compute scaling factors for parameters
		float search_area_factor = frame_width*6.0f/320.0f/frame_rate;
		float ttl_factor = frame_rate/6.0f;
		// Adapt parameters
		parameters.set<int>("searchAreaIncrementForObject", parameters.get<int>("searchAreaIncrementForObject")*search_area_factor);
		parameters.set<int>("searchAreaIncrementForDirection", parameters.get<int>("searchAreaIncrementForDirection")*search_area_factor);
		parameters.set<int>("objectTTL", parameters.get<int>("objectTTL")*ttl_factor); // Assuming default frame rate equal to 6, rather than 5 or 8
	}
		
	// Draw objects' tracks.
	// The input matrix should be the frame on which to draw.
	void CovarianceTracker::drawTracks(Mat& bg)
	{
		// Call parent method, which draws all trajectories
		Tracker::drawTracks(bg);
		// Define the colors to use
		//int num_colors = 3;
		//Scalar colors[] = {CV_RGB(255,0,0), CV_RGB(0,255,0), CV_RGB(0,0,255), CV_RGB(255,255,0), CV_RGB(255,0,255), CV_RGB(0,255,255)};
		// Draw each tracked object's current searching area, if required
		if(!drawSearchAreaFlag)
			return;
		for(vector<TrackedObject*>::const_iterator it = tracked_objects.begin(); it != tracked_objects.end(); it++)
		{
			// Get tracked object reference
			const CovarianceTrackedObject& tracked_object = *((CovarianceTrackedObject*) (*it));
			// Get color
			srand(tracked_object.id);
			Scalar object_color = CV_RGB(rand()*255, rand()*255, rand()*255);//colors[tracked_object.id % num_colors];
			// Compute tracked object's search regions
			Rect t_rect, dir_rect;
			computeSearchRegions(tracked_object, t_rect, dir_rect);
			// Draw regions
			int thickness = 1;
			rectangle(bg, Point(t_rect.x, t_rect.y), Point(t_rect.x + t_rect.width - 1, t_rect.y + t_rect.height - 1), object_color, thickness);
			//rectangle(bg, Point(dir_rect.x, dir_rect.y), Point(dir_rect.x + dir_rect.width - 1, dir_rect.y + dir_rect.height - 1), object_color, thickness);
		}
	}

}
