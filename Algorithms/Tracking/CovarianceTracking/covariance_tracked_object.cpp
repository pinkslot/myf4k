// Tracked object for covariance tracking. Manages information about the object's model, such as its covariance matrices.
// TODO use parent TrackedObject::addDetection()

#include "covariance_tracked_object.h"
#include <math_utils.h>
#include <my_exception.h>
#include <shell_utils.h>
#include <log.h>

// Debug includes
#include <iostream>
using namespace std;

namespace alg
{

	// Debug: initialize next object id
	long int CovarianceTrackedObject::next_id = 0;

	// Constructor, requires covariance matrix queue size and initial TTL 
	CovarianceTrackedObject::CovarianceTrackedObject(int init_queue_size, int init_ttl)
	{
		// Debug: assign id
		id = next_id++;
		// Initialize TTL variables
		initial_ttl = init_ttl;
		current_ttl = init_ttl;
		// Initialize average distance variables
		distance_sum = 0;
		distance_samples = 0;
		// Initialize queue
		queue_size = init_queue_size;
		covariance_matrices = new Mat[queue_size];
		queue_next_element = 0;
		queue_full = false;
		// Set freed to false
		freed = false;
	}
		
	// Returns the coordinates of the last position of the object
	Point2i CovarianceTrackedObject::currentPosition()
	{
		// Get the object's last region
		const cvb::CvBlob& region = detected_regions.back();
		return Point2i(region.x + region.width()/2, region.y + region.height()/2);
	}

	// Get last moving distance
	int CovarianceTrackedObject::getLastMovingDistance() const
	{
		if(detected_regions.size() > 1)
		{
			// Get iterator to end
			vector<cvb::CvBlob>::const_reverse_iterator it = detected_regions.rbegin();
			// Get current blob
			const cvb::CvBlob& blob = *it;
			// Get previous blob
			++it;
			const cvb::CvBlob& prev_blob = *it;
			// Compute distance between blobs
			Point p1((blob.minx + blob.maxx)/2, (blob.miny + blob.maxy)/2);
			Point p2((prev_blob.minx + prev_blob.maxx)/2, (prev_blob.miny + prev_blob.maxy)/2);
			float distance = sqrt((p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y));
			return distance;
		}
		return 0;
	}
		
	// Check if the last two positions are in consecutive frames
	bool CovarianceTrackedObject::lastMovementIsConsecutive() const
	{
		if(detected_frames.size() > 1)
		{
			// Get iterator to end
			vector<int>::const_reverse_iterator it = detected_frames.rbegin();
			// Get current frame
			int curr_frame = *it;
			// Get previous frame
			++it;
			int prev_frame = *it;
			Log::debug() << "Checking if last movement is consecutive: current frame " << curr_frame << ", previous frame " << prev_frame << endl;
			// Check if they are consecutive
			if(prev_frame == curr_frame - 1)
			{
				return true;
			}
		}
		return false;
	}
		
	// Notify the object has been detected in a frame, add a new covariance matrix and reset ttl.
	// If the given frame number has been assigned to the object, ignore it.
	void CovarianceTrackedObject::addDetection(int frame_num, const cvb::CvBlob& detection_blob, const Mat& covariance_matrix)
	{
		Log::debug() << "Adding detection: " << detection_blob.x << ", " << detection_blob.y << ", " << detection_blob.width() << ", " << detection_blob.height() << endl;
		//Log::debug() << "Cov mat: " << covariance_matrix << endl << endl;
		// Check if we already have data for this frame
		if(detected_frames.size() > 0 && detected_frames.back() == frame_num)
		{
			// Ignore this region
			return;
		}
		// Reset the ttl to the initial value
		current_ttl = initial_ttl;
		// Save the frame number and detection in region in the appropriate vectors
		detected_frames.push_back(frame_num);
		cvb::CvBlob copy;
		cvCloneBlob(detection_blob, copy);
		detected_regions.push_back(copy);
		// If we have two consecutive detections, add the distance to distance_sum
		if(detected_regions.size() > 1)
		{
			// Get previous frame number
			int prev_frame_num = *(detected_frames.rbegin() + 1);
			// Check if they are consecutive
			if(prev_frame_num == frame_num - 1)
			{
				// Update average distance variables
				distance_sum += getLastMovingDistance();
				++distance_samples;
			}
		}
		// Add the covariance matrix to the queue
		covariance_matrices[queue_next_element] = covariance_matrix;
		// Increase the next_element pointer and check if the queue is full
		queue_next_element++;
		if(queue_next_element == queue_size)
		{
			// Reset the pointer
			queue_next_element = 0;
			// Notify full queue (when the queue is not full, queue_next_element is used to check how many elements have been inserted)
			queue_full = true;
		}
		// Compute the new mean covariance matrix
		// Get the number of matrices we are going to consider
		int num_elements = queue_full ? queue_size : queue_next_element;
		// Check if we already have valid data for this object
		bool surely_valid_model = (detected_frames.size() > 1);
		// Create reference to current model or to the new covariance matrix if this is the first detection
		const Mat& current_model = (detected_frames.size() > 1 ? current_mean_matrix : covariance_matrix);
		// Check no errors occur in the computation; otherwise reset the matrix list and keep using the current model
		try
		{
			current_mean_matrix = MathUtils::covarianceLieMean(covariance_matrices, num_elements, current_model, false);
		}
		catch(MyException& e)
		{
			// Check if we already have some valid data to start over with
			if(surely_valid_model)
			{
				// Reset the queue and insert the current mean as the first matrix
				Log::error() << ShellUtils::RED << "CovarianceTrackedObject:addDetection(): error: " << e.what() << endl;
				queue_next_element = 1;
				num_elements = 1;
				queue_full = false;
				covariance_matrices[0] = current_mean_matrix.clone();
				Log::error() << "Resetting the object..." << ShellUtils::NORMAL << endl;
			}
			else
			{
				// Throw something, the caller will probably decide to discard this object
				Log::error() << ShellUtils::RED << "CovarianceTrackedObject::addDetection(): error: " << e.what() << endl;
				Log::error() << "We have no valid data to start over with, discard object." << ShellUtils::NORMAL << endl;
				stringstream error;
				error << "CovarianceTrackedObject::addDetection(): error: " << e.what();
				throw MyException(error.str());
			}
		}
	}
		
	// Decrease ttl, returns true if ttl == 0.
	bool CovarianceTrackedObject::decreaseTTL()
	{
		// Check if TTL is already 0; it shouldn't happen, but you never know
		if(current_ttl > 0)
		{
			current_ttl--;
		}
		// Return true if TTL == 0
		return current_ttl == 0;
	}

	// Notify that the object has been lost and free not needed memory
	void CovarianceTrackedObject::stopTracking()
	{
		// Free queue memory
		if(! freed)
		{
			freed = true;
			delete [] covariance_matrices;
		}
	}
		
	// Destructor for freeing the matrix queue, just calls stopTracking()
	CovarianceTrackedObject::~CovarianceTrackedObject()
	{
		stopTracking();
	}

}
