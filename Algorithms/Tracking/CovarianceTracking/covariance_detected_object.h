// Class which represents and object which has been detected but not yet classified (i.e. assigned to an existing object or identified as a new one).

#ifndef COVARIANCE_DETECTED_OBJECT_H
#define COVARIANCE_DETECTED_OBJECT_H

#include <cv.h>
#include "../detected_object.h"

using namespace cv;

namespace alg
{

	class CovarianceDetectedObject : public DetectedObject
	{
		protected:
		
		// Covariance matrix of the object
		Mat covariance_matrix;

		public:

		// Constructor with inputs for the covariance matrix and the object region
		inline CovarianceDetectedObject(const Mat& covar_mat, const cvb::CvBlob& rect) :
			covariance_matrix(covar_mat) { cvb::cvCloneBlob(rect, region_); assigned_ = false; }

		// Copy constructor
		inline CovarianceDetectedObject(const CovarianceDetectedObject& copy) :
			covariance_matrix(copy.covariance_matrix) { cvb::cvCloneBlob(copy.region_, region_); assigned_ = copy.assigned_; }

		// Accessor method for covariance_matrix
		inline Mat& covarianceMatrix() { return covariance_matrix; }
		
	};

}

#endif
