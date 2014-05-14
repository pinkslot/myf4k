// Main executable for the detection and tracking of fish from an input video.
// Common includes
#include <iostream>
#include <cv.h>
#include <time_utils.h>
#include <math_utils.h>
#include <shell_utils.h>
#include <results_utils.h>
#include <opencv_utils.h>
#include <tclap/CmdLine.h>
#include <log.h>
#include <results.h>
#include <xmltdresults.h>
#include <context.h>
#include <covariance_tracker.h>

using namespace cv;
using namespace std;
using namespace alg;
using namespace TCLAP;

int main(int argc, char** argv)
{
	try
	{
		// Define variables for command-line parameters
		string video_path;
		string gt_path;
		// Initialize log
		Log::setDebugLogStream(cout);
		Log::setInfoLogStream(cout);
		Log::setWarningLogStream(cout);
		Log::setErrorLogStream(cerr);
		// Define command-line parameters for TCLAP
		try
		{
			// Define cmd object
			CmdLine cmd("Usage: ", ' ', "0.1.0");
			// Video path
			ValueArg<string> video_path_arg("v", "video", "Input video path", true, "video.mpg", "path", cmd);
			// Ground truth
			ValueArg<string> gt_path_arg("g", "gt", "Path to ground truth XML file", false, "", "path", cmd);
			// Parse actual arguments
			cmd.parse(argc, argv);
			// Read easy values
			video_path = video_path_arg.getValue();
			gt_path = gt_path_arg.getValue();
		}
		catch (ArgException& e)
		{
			Log::error() << "Error while parsing command line arguments. " << e.error() << " for arg " << e.argId() << endl;
		       exit(1);
		}
		catch (...)
		{
			Log::error() << "Error while parsing command line arguments. " << endl;// << e.error() << " for arg " << e.argId() << endl;
		       exit(1);
		}
		// Open the video file
		VideoCapture video_cap(video_path);
		// Check for errors
		if(!video_cap.isOpened())
		{
			Log::error() << "Error opening the video file." << endl;
			exit(1);
		}
		// Load ground truth results
		Results gt_results;
		try
		{
			XMLTDResults xmltd;
			gt_results = xmltd.readFile(gt_path);
		}
		catch(MyException& e)
		{
			Log::e() << ShellUtils::RED << "Error reading ground-truth results." << ShellUtils::NORMAL << endl;
			exit(1);
		}
		// Create tracker
		CovarianceTracker tracker;
		FeatureExtractorSet *covariance_feature_extractor = new FeatureExtractorSet();
		covariance_feature_extractor->addExtractor(new XYFeatureExtractor());
		covariance_feature_extractor->addExtractor(new RGBFeatureExtractor());
		//covariance_feature_extractor->addExtractor(new HueFeatureExtractor());
		covariance_feature_extractor->addExtractor(new DerivFeatureExtractor(1,0));
		covariance_feature_extractor->addExtractor(new DerivFeatureExtractor(0,1));
		//covariance_feature_extractor->addExtractor(new HistFeatureExtractor());
		tracker.featureExtractor = covariance_feature_extractor;
		// Define performance variables
		float sum_region_overlap = 0.0f;
		float num_tests = 0;
		// Start processing cycle
		bool first = true;
		int frame_num;
		Mat frame, next_frame;
		for(frame_num=0; true; frame_num++)
		{
			try
			{
				if(first)
				{
					first = false;
					video_cap >> next_frame;
				}
				frame = next_frame.clone();
				video_cap >> next_frame;
				// Check frame validity
				if(frame.rows == 0 || frame.cols == 0 || frame.data == NULL)
				{
					break;
				}
				Log::d() << "Processing frame " << frame_num << endl;
				// Check if we have objects in this frame and next
				ObjectList frame_objects = gt_results.getObjectsByFrame(frame_num);
				Log::i() << "Objects in this frame: " << frame_objects.size() << endl;
				// Go through each object
				for(ObjectList::iterator o_it = frame_objects.begin(); o_it != frame_objects.end(); o_it++)
				{
					// Get all appearances for this object
					ObjectList objects = gt_results.getObjectsById(o_it->getId());
					// Pointers to the object in this frame and in the next frame
					Object* curr_object = NULL;
					Object* next_object = NULL;
					// Get current object and next object
					for(ObjectList::iterator o_it2 = objects.begin(); o_it2 != objects.end(); o_it2++)
					{
						// Get reference to object
						Object& object = *o_it2;
						// Check frame number
						if(object.getFrameNumber() == frame_num)	curr_object = &object;
						if(object.getFrameNumber() == frame_num+1)	next_object = &object;
					}
					// Check pointers
					if(curr_object == NULL || next_object == NULL)
					{
						// Skip
						continue;
					}
					Log::i() << "Object " << curr_object->getId() << " found in current and next frame." << endl;
					// Get rectangles containing objects
					int curr_min_x, curr_max_x, curr_min_y, curr_max_y;
					int next_min_x, next_max_x, next_min_y, next_max_y;
					minMaxCoords(curr_object->getBoundingBox(), &curr_min_x, &curr_max_x, &curr_min_y, &curr_max_y);
					minMaxCoords(next_object->getBoundingBox(), &next_min_x, &next_max_x, &next_min_y, &next_max_y);
					Rect curr_rect(curr_min_x, curr_min_y, curr_max_x-curr_min_x+1, curr_max_y-curr_min_y+1);
					Rect next_rect(next_min_x, next_min_y, next_max_x-next_min_x+1, next_max_y-next_min_y+1);
					Log::d() << "Curr rect: " << curr_rect.x << " " << curr_rect.y << " " << curr_rect.width << " " << curr_rect.height << endl;
					Log::d() << "Next rect: " << next_rect.x << " " << next_rect.y << " " << next_rect.width << " " << next_rect.height << endl;
					// Compute model for object in current frame
					Log::i() << "Computing covariance model." << endl;
					cvb::CvBlob* blob = createBlob(curr_object->getContour());
					Mat mask = blobToMat(*blob, CV_POSITION_ORIGINAL, true, frame.cols, frame.rows);
					delete blob;
					Mat model = MathUtils::computeCovarianceMatrix(tracker.featureExtractor->computeFeatureSet(frame, mask));
					// Compute covariance matrix for correct location in the next frame
					Mat next_model = MathUtils::computeCovarianceMatrix(tracker.featureExtractor->computeFeatureSet(frame(next_rect)));
					// Draw current frame
					//Mat draw_curr_frame = frame.clone();
					//rectangle(draw_curr_frame, Point(curr_min_x, curr_min_y), Point(curr_max_x, curr_max_y), CV_RGB(0,255,0));
					// Compute search area
					int search_min_x = max(0, min(curr_min_x, next_min_x) - 20);
					int search_max_x = min(frame.cols-1, max(curr_max_x, next_max_x) + 20);
					int search_min_y = max(0, min(curr_min_y, next_min_y) - 20);
					int search_max_y = min(frame.rows-1, max(curr_max_y, next_max_y) + 20);
					Rect search_rect(search_min_x, search_min_y, search_max_x-search_min_x+1, search_max_y-search_min_y+1);
					// Search object
					int avg_search_size = max(curr_rect.width, curr_rect.height);
					Log::i() << "Searching object in next frame." << endl;
					Rect result = tracker.findObject(model, frame, search_rect, avg_search_size-1, avg_search_size+1, 2);
					Log::i() << "Distance from correct location: " << MathUtils::covarianceDistance(model, next_model) << endl;
					// Check validity
					//bool result_valid = true;
					if(result.width == 0 || result.height == 0)
					{
						Log::warning("No match found.");
						//result_valid = false;
					}
					// Compute overlap
					Rect intersect_rect = result & next_rect;
					Rect union_rect = result | next_rect;
					float overlap = ((float)intersect_rect.area())/((float)union_rect.area());
					Log::i() << "Overlap: " << overlap << endl;
					sum_region_overlap += overlap;
					num_tests++;
					// Draw results
					//Mat draw_next_frame = next_frame.clone();
					//rectangle(draw_next_frame, Point(search_min_x, search_min_y), Point(search_max_x, search_max_y), CV_RGB(0,0,128));
					//if(result_valid) rectangle(draw_next_frame, Point(result.x, result.y), Point(result.x+result.width-1, result.y+result.height-1), CV_RGB(0,0,255));
					//rectangle(draw_next_frame, Point(next_min_x, next_min_y), Point(next_max_x, next_max_y), CV_RGB(0,255,0));
					// Show images
					//imshow("curr", draw_curr_frame);
					//imshow("next", draw_next_frame);
					//waitKey(0);
				}
			}
			catch(MyException& e)
			{
				Log::error() << ShellUtils::RED << "Error in main at frame " << frame_num << ": " << e.what() << ShellUtils::NORMAL << endl;
				//exit(1);
			}
			catch(...)
			{
				Log::error() << ShellUtils::RED << "Error in main at frame " << frame_num << "." << ShellUtils::NORMAL << endl;
				//exit(1);
			}
		}
		// Compute average region overlap
		Log::i() << "Average region overlap: " << sum_region_overlap/num_tests << endl;
		// Done!
		Log::info(0) << "Done!" << endl;
	}
	catch(MyException& e)
	{
		Log::error() << ShellUtils::RED << "Error: " << e.what() << ShellUtils::NORMAL << endl;
	}
	catch(...)
	{
		Log::error() << ShellUtils::RED << "Error." << ShellUtils::NORMAL << endl;
	}
}
