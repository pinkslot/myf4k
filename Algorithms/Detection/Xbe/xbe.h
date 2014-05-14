#ifndef XBE_H
#define XBE_H

#include "../detection.h"
#include <vector>
#include <matrix.h>

namespace alg
{

	class Xbe : public Detection
	{
		public:

		// Define a pixel value as a set of features
		typedef std::vector<float> Xvalue;
		// Define a pixel as the history of Xvalue
		typedef std::vector<Xvalue> Xpixel;
		
		private:

		// Matrix of Xpixels
		Matrix<Xpixel> background;

		// Cached value of feature_mask
		long int feature_mask;

		// Check if a feature is selected - uses cached feature_mask value
		inline bool checkFeature(long int feature_id) { return (feature_mask & feature_id) > 0; }
	
		// Convert image to Xvalue matrix
		Matrix<Xvalue> convertImage(const cv::Mat& frame);

		public:
		
		// Feature bits
		static const long int RGB_R =		1 << 0;
		static const long int RGB_G =		1 << 1;
		static const long int RGB_B =		1 << 2;
		static const long int HSV_H =		1 << 3;
		static const long int HSV_S =		1 << 4;
		static const long int HSV_V =		1 << 5;
		static const long int Lab_L =		1 << 6;
		static const long int Lab_a =		1 << 7;
		static const long int Lab_b =		1 << 8;
		static const long int YIQ_Y =		1 << 9;
		static const long int YIQ_I =		1 << 10;
		static const long int YIQ_Q =		1 << 11;
		static const long int GS =		1 << 12;
		
		inline std::string name() const { return "xbe"; }
		inline std::string description() const { return "Extended Background Extractor"; }
		inline std::string version() const { return "1.0"; }
		inline int executionTime() const { return 0; }
		inline int ram() const { return 0; }

		// Constructor
		Xbe();

		// Destructor, for memory deallocation
		~Xbe();

		// Initialize model from frame
		void initModel(const Matrix<Xvalue>&);
		
		// Update model from frame and foreground mask
		void updateModel(const Matrix<Xvalue>&, const cv::Mat&);

		// Analyze next frame and return objects:
		cv::Mat nextFrame(Context& context);
	};

}

#endif
