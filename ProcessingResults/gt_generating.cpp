#include "gt_generating.h"
#include <db_helper.h>
#include <log.h>
#include <cvblob.h>
#include <iostream>
#include <highgui.h>
#include <string>
#include <opencv_utils.h>
#include <sstream>


using namespace std;
using namespace cv;


void on_mouse(int evt, int x, int y, int flags, void* param)
{
	GtGenerating *gt_gen = (GtGenerating *)param;
	const Context &context = gt_gen->getContext();

    if(evt==CV_EVENT_LBUTTONDOWN || evt==CV_EVENT_RBUTTONDOWN)
    {

    	ObjectList objs = context.results.getObjectsByFrame(context.frame_num);
    	// find clicked object
		for (ObjectList::iterator p_obj = objs.begin(); p_obj != objs.end(); ++p_obj)
		{
			if (cv::pointPolygonTest(p_obj->getContour(), Point2f(x, y), false) > 0)
			{
				Mat draw_frame = context.frame;
		    	string type;
		    	// fish if left-click
		    	if (evt==CV_EVENT_LBUTTONDOWN)
		    		type = "fish";
		    	// nofish if right-click																																																																																																												
		    	else if (evt==CV_EVENT_RBUTTONDOWN)
		    		type = "nofish";
		    	
		    	p_obj->setType(type);
		    	gt_gen->getResults().addObject(*p_obj);
			    std::stringstream ss(context.file_name);
			    std::string item;
			    vector<string> elems;
			    while (std::getline(ss, item, '/')) {
			        elems.push_back(item);
			    }
			    string fname = elems.back();	    	
		    	stringstream sname;
			    sname << fname << context.frame_num << p_obj->getId();
			    string name = sname.str();
				Mat mask = Mat::zeros(draw_frame.size(), CV_8UC1);
				drawContour(p_obj->getContour(), mask, Scalar(255,255,255,255), CV_FILLED);
				Log::info(0) << name << endl;
				imwrite("GroundTruth/" + type + "/" + name + "_src.bmp", context.frame);
				imwrite("GroundTruth/" + type + "/" + name + "_msk.bmp", mask);		
				return;
			}
		}
		Log::info(0) << "miss contour" <<endl;
	}
}

void GtGenerating::saveResult(db::DBConnection *d)
{

	// Add another field!
	Log::info(0) << "save fish ground truth with id :" 
		<< db::DBHelper::saveGTResults(d, results, "name", "type", "gt_class", 1, "gt_video_id") 
		<< endl;
}

GtGenerating::GtGenerating(const Context &c): context(c)
{
	namedWindow("frame_win");
	cvSetMouseCallback("frame_win", &on_mouse, this);
}
