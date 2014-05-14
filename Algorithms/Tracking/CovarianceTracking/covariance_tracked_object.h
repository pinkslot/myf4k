// Tracked object for covariance tracking. Manages information about the object's model, such as its covariance matrices.

#ifndef COVARIANCE_TRACKED_OBJECT_H
#define COVARIANCE_TRACKED_OBJECT_H

#include <cv.h>
#include <cvblob.h>
#include <vector>
#include "../tracked_object.h"

using namespace cv;

namespace alg
{

	class CovarianceTrackedObject : public TrackedObject
	{
		protected:

		// Variables to keep track of the average distance by which the object has moved
		int distance_sum;
		int distance_samples;
		
		// Counter for the number of frames after which the object, if not detected, will be removed
		int current_ttl;
		int initial_ttl;

		// Circular queue of covariance matrices
		Mat* covariance_matrices;

		// Variables for the management of the circular queue
		int queue_size;
		int queue_next_element;
		bool queue_full;

		// Current mean covariance matrix (so that it is computed only once, when a new covariance matrix is given)
		Mat current_mean_matrix;

		// Check if the object has already been freed
		bool freed;

		// Debug: next object identifier
		static long int next_id;

		public:

		// Constructor, requires initial queue size and initial TTL
		// - init_queue_size: size of the covariance matrix queue
		// - init_ttl: maximum TTL value
		CovarianceTrackedObject(int init_queue_size, int init_ttl);

		// Returns the mean covariance matrix computed so far
		inline const Mat& covarianceModel() { return current_mean_matrix; }

		// Returns the coordinates of the last position of the object
		Point2i currentPosition();

		// Returns the objet's TTL
		inline int TTL() const { return current_ttl; }

		// Notify the object has been detected in a frame, add a new covariance matrix and reset ttl.
		// If the given frame number has been assigned to the object, ignore it.
		virtual void addDetection(int, const cvb::CvBlob&, const Mat&);

		// Get average distance by which this object moves
		inline int getAverageMovingDistance() const { return distance_sum == 0 ? 0 : distance_sum/distance_samples; }

		// Check if the last two positions are in consecutive frames
		bool lastMovementIsConsecutive() const;

		// Get last moving distance
		int getLastMovingDistance() const;

		// Decrease ttl, returns true if ttl == 0.
		bool decreaseTTL();

		// Notify that the object has been lost and free not needed memory
		void stopTracking();

		// Destructor for freeing the matrix queue
		virtual ~CovarianceTrackedObject();
	};

}

#endif
