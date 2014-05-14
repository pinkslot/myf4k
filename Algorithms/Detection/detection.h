// Interface for a generic motion detection class.

#ifndef ALG_DETECTION_H
#define ALG_DETECTION_H

#include <cv.h>
#include <context.h>
#include "../algorithm.h"

namespace alg
{

	class Detection : public Algorithm
	{
		public:

		// Analyze next frame and return motion mask
		virtual cv::Mat nextFrame(Context& context) = 0;

		// Required by Algorithm interface
		std::string name() const = 0;
		inline std::string type() const { return "detection"; }
		std::string description() const = 0;
		std::string version() const = 0;
		int executionTime() const = 0;
		std::string descriptionExecutionTime() const { return "Average time to process a 640x480 frame"; }
		int ram() const = 0;
		inline std::string input() const { return "cv::Mat"; }
		inline std::string output() const { return "cv::Mat"; }
		
		virtual ~Detection() {}

		// Check if algorithm is ready to return meaningful output
		virtual bool isReady() { return true; }

		virtual void adaptToVideo(int frame_width, int frame_height, int frame_rate) {}

	};

}

#endif
