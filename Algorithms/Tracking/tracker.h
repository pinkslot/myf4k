// Abstract class for object tracking.

#ifndef TRACKER_H
#define TRACKER_H

#include <cv.h>
#include <highgui.h>
#include <cvblob.h>
#include <vector>
#include "tracked_object.h"
#include "../algorithm.h"

namespace alg
{

	typedef std::vector<TrackedObject*> TrackedObjectList;
	typedef std::pair<TrackedObject*, DetectedObject*> TrackingDetectionPair;
	typedef std::pair<TrackingDetectionPair, float> TrackingDetectionCandidate;

	class Tracker : public Algorithm
	{
		protected:
		
		// List of currently tracked objects
		TrackedObjectList tracked_objects;
		
		// List of previously tracked objects which have been lost
		TrackedObjectList lost_objects;
		
		// Function for comparing two TrackingDetectionCandidate objects
		inline static bool compareCandidates(const TrackingDetectionCandidate& c1, const TrackingDetectionCandidate& c2) { return c1.second < c2.second; }

		public:
		
		std::string name() const = 0;
		inline std::string type() const { return "tracking"; }
		std::string description() const = 0;
		std::string version() const = 0;
		int executionTime() const = 0;
		std::string descriptionExecutionTime() const { return "Average time to process a tracked object in a single frame"; }
		int ram() const = 0;
		inline std::string input() const { return "cvb::CvBlobs"; }
		inline std::string output() const { return "std::vector<alg::TrackedObject>"; }
		
		virtual ~Tracker() {}
		
		// Returns the number of currently tracked objects
		virtual inline int numTrackedObjects() { return tracked_objects.size(); }

		// Returns the list of currently tracked objects
		virtual std::vector<const TrackedObject*> getTrackedObjects()
		{
			// Define output vector
			std::vector<const TrackedObject*> result;
			// Fill output vector
			for(unsigned int i=0; i<tracked_objects.size(); i++)
			{
				result.push_back((const TrackedObject*) (tracked_objects[i]));
			}
			// Return result
			return result;
		}
		
		// Returns the list of previously tracked objects
		virtual std::vector<const TrackedObject*> getLostObjects()
		{
			// Define output vector
			std::vector<const TrackedObject*> result;
			// Fill output vector
			for(unsigned int i=0; i<lost_objects.size(); i++)
			{
				result.push_back((const TrackedObject*) (lost_objects[i]));
			}
			// Return result
			return result;
		}

		// Read next frame, frame number, object blobs, binary masks and perform tracking.
		// The i-th element of the mask vector must have the same size as the i-th element of the rectangle vector.
		// The input image type is supposed to be CV_8UC3.
		// - image: new image
		// - frame_number: frame number of the given image
		// - blobs: list of blobs
		virtual void nextFrame(const cv::Mat& image, int frame_number, const cvb::CvBlobs& blobs) = 0;

		// Adapt tracking parameters to video properties
		virtual void adaptToVideo(cv::VideoCapture& video_cap) = 0;

		// Draw objects' tracks.
		// - image: image on which to draw the tracks (it should be a copy of the last frame)
		virtual void drawTracks(cv::Mat& bg)
		{
			// Define the colors to use
			//int num_colors = 3;
			//Scalar colors[] = {CV_RGB(255,0,0), CV_RGB(0,255,0), CV_RGB(0,0,255)};
			// Draw each tracked object's current track
			for(vector<TrackedObject*>::const_iterator it = tracked_objects.begin(); it != tracked_objects.end(); it++)
			{
				// Get tracked object reference
				const TrackedObject& tracked_object = *(*it);
				// Get object's data
				const std::vector<int>& frames = tracked_object.frameList();
				const std::vector<cvb::CvBlob>& regions = tracked_object.regionList();
				// Read number of appearances
				int num_points = frames.size();
				// Define counter for frames and regions
				int current = 0;
				// Get first position
				cv::Point2i start(regions[current].x + regions[current].width()/2, regions[current].y + regions[current].height()/2);
				// Get color
				srand(tracked_object.id);
				cv::Scalar object_color = CV_RGB(rand()*255, rand()*255, rand()*255);//colors[tracked_object.id % num_colors];
				// Go through all points
				for(current=1; current<num_points; current++)
				{
					// Get current position
					cv::Point2i end(regions[current].x + regions[current].width()/2, regions[current].y + regions[current].height()/2);
					// Draw the line
					cv::line(bg, start, end, object_color, 2);
					// Set current end point as new start point
					start = end;
				}
				// Draw object's tracking area
				//Rect region = tracked_object.currentRegion();
				//int increase = (parameters.objectTTL - tracked_object.TTL())*parameters.searchAreaIncrement;
				//region -= Point(increase, increase);
				//region += Size(2*increase, 2*increase);
				//int thickness = (tracked_object.TTL() == parameters.objectTTL ? 2 : 1);
				//rectangle(bg, Point(region.x, region.y), Point(region.x + region.width, region.y + region.height), object_color, thickness);
			}
		}
	};

}

#endif
