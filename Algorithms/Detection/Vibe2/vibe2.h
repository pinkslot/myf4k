#ifndef VIBE2_H
#define VIBE2_H

#include <detection.h>
#include <cvblob.h>
#include <cv.h>
#include "vibe-background.h"

namespace alg
{

	class ViBE2 : public Detection
	{
		private:

		// Initialization flag
		bool init;
		// Background model
		vibeModel_t* model;
		// Image data
		uint8_t* image_data;
		// Segmentation map
		uint8_t* segmentation_map;
		// Image dimensions
		int height, width, stride;
		// Array of learning matrices
		cv::Mat* learning_buffer;
		
		public:
		
		inline string name() const { return "vibe"; }
		inline string description() const { return "Visual Background extractor (ViBe)"; }
		inline string version() const { return "1.1"; }
		inline int executionTime() const { return 0; }
		inline int ram() const { return 0; }

		ViBE2(void);
		~ViBE2(void);

		cv::Mat nextFrame(Context& context);
		
	};

}

#endif
