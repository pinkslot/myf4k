// Generate GroundTruth
#ifndef GT_GENERATING_H
#define GT_GENERATING_H

#include "results.h"
#include <context.h>
#include <db_helper.h>
#include <cv.h>

using namespace pr;

class GtGenerating
{
private:
	// fields
	const Context &context;
	// db::DBConnection db_conn;	
	Results results;

public:
	inline const Context &getContext() { return context;}
	inline Results &getResults() { return results;}

	GtGenerating(const Context &c);
	inline void nextFrame(cv::Mat frame) 
	{
		imshow("frame_win", frame);
	}
	void saveResult(db::DBConnection *d);
};

#endif
