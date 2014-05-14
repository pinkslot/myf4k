// Class for utility functions for DB interaction
#include "db_helper.h"
#include "fish_detection.h"
#include "processed_video.h"
#include "ground_truth.h"
#include "gt_object.h"
#include "gt_keyframes.h"
#include "perla_ground_truth.h"
#include "perla_gt_object.h"
#include "perla_best_ground_truth.h"
#include "perla_gt_best_object.h"
#include <results.h>
#include <sstream>
#include <cv.h>

using namespace cv;
using namespace std;

namespace db
{
	
	// Save ground truth	
	int DBHelper::saveGTResults(DBConnection* connection, const pr::Results& results, std::string name, std::string type, std::string gt_class, int gt_agent_id, std::string gt_video_id)
	{
		// Create ground truth record
                Log::i() << "Adding ground truth record." << endl;
                GroundTruth ground_truth(connection);
                ground_truth.setField<string>("name", name);
                ground_truth.setField<string>("type", type);
                ground_truth.setField<string>("class", gt_class);
                ground_truth.setField<int>("agent_id", gt_agent_id);
                ground_truth.setField<string>("video_id", gt_video_id);
                ground_truth.insertToDB();
                // Get ground truth id
                int ground_truth_id = ground_truth.getField<int>("id");
                // Add keyframes
                Log::i() << "Adding keyframes." << endl;
                list<KeyFrameInterval> kf_intervals = results.getKeyFrames().getIntervals();
                for(list<KeyFrameInterval>::iterator kf_it = kf_intervals.begin(); kf_it != kf_intervals.end(); kf_it++)
                {
                        // Get interval
                        KeyFrameInterval& kf = *kf_it;
                        // Create entity
                        GtKeyFrames gt_kf(connection);
                        gt_kf.setField<int>("ground_truth_id", ground_truth_id);
                        gt_kf.setField<int>("start", kf.start());
                        gt_kf.setField<int>("end", kf.end());
                        gt_kf.setField<string>("label", kf.label);
                        // Add to db
                        gt_kf.insertToDB();
                }
                // Add objects
                Log::i() << "Adding objects." << endl;
                int next_detection_id = 1;
                list<int> frames = results.frameList();
                for(list<int>::iterator f_it = frames.begin(); f_it != frames.end(); f_it++)
                {
                        // Get objects in frame
			pr::ObjectList objects = results.getObjectsByFrame(*f_it);
                        for(pr::ObjectList::iterator o_it = objects.begin(); o_it != objects.end(); o_it++)
                        {
                                // Get object
				pr::Object& object = *o_it;
                                // Create record
                                GtObject gt_object(connection);
                                gt_object.setField<int>("detection_id", next_detection_id++);
                                gt_object.setField<int>("object_id", object.getId());
                                gt_object.setField<int>("ground_truth_id", ground_truth_id);
                                gt_object.setField<string>("type", object.getType());
                                gt_object.setField<bool>("optional", object.getOptional());
                                gt_object.setField<int>("frame_number", object.getFrameNumber());
				/*cout << object.getContour()[0].x << " " << object.getContour()[0].y << endl;
				cout << object.getContour()[1].x << " " << object.getContour()[1].y << endl;
				cout << object.getContour()[2].x << " " << object.getContour()[2].y << endl;
				cout << object.getContour()[3].x << " " << object.getContour()[3].y << endl;
				cout << object.getContour()[4].x << " " << object.getContour()[4].y << endl;
				vector<Point> bb = boundingBoxFromContour(object.getContour());
				string enc = ContourEncoder::encode(bb, object.getContour());
				vector<Point> dec = ContourEncoder::decode(enc);
				cout << dec[0].x << " " << dec[0].y << endl;
				cout << dec[1].x << " " << dec[1].y << endl;
				cout << dec[2].x << " " << dec[2].y << endl;
				cout << dec[3].x << " " << dec[3].y << endl;
				cout << dec[4].x << " " << dec[4].y << endl;
				exit(0);*/
                                gt_object.setField<string>("contour", ContourEncoder::encode(object.getContour()));
                                // Add to db
                                gt_object.insertToDB();
                        }
                }
		// Return ground truth id
		return ground_truth_id;
	}

	// Get results by processed video id
	pr::Results DBHelper::getFDResults(DBConnection* connection, int processed_video_id)
	{
		// Define output
		pr::Results results;
		// Get processed video data
		ProcessedVideo processed_video(connection);
		processed_video.setField<int>("id", processed_video_id);
		processed_video.readFromDB();
		// Get detections for that video
		FishDetection source(connection);
		source.setField<string>("video_id", processed_video.getField<string>("video_id"));
		source.setField<int>("component_id", processed_video.getField<int>("component_id"));
		vector<FishDetection*> fish_detections = connection->queryList(source, vector<string>(), "ORDER BY frame_id");
		// Add each detection
		int num_detections = fish_detections.size();
		int detection_count = 1;
		for(vector<FishDetection*>::iterator f_it = fish_detections.begin(); f_it != fish_detections.end(); f_it++)
		{
			Log::d() << "\rReading object " << detection_count++ << "/" << num_detections;
			Log::d().flush();
			// Get reference to fish detection
			FishDetection& detection = **f_it;
			// Create object
			pr::Object object;
			// Read bounding box and contour
			vector<Point> contour = ContourEncoder::decode(detection.getField<string>("bb_cc"));
			// Set data
			object.setId(detection.getField<int>("fish_id"));
			object.setFrameNumber(detection.getField<int>("frame_id"));
			object.setType("fish");
			//object.setBoundingBox(bounding_box);
			object.setContour(contour);
			object.setDetectionCertainty(detection.getField<float>("detection_certainty"));
			object.setTrackingCertainty(detection.getField<float>("tracking_certainty"));
			// Add object
			results.addObject(object);
		}
		Log::d() << endl;
		// Return results
		return results;
	}
	
	// Get results from ground_truth/gt_object tables
	pr::Results DBHelper::getPerlaBestGTResults(DBConnection* connection, int ground_truth_id)
	{
		// Define output
		pr::Results results;
		// Get processed video data
		//ProcessedVideo processed_video(connection);
		//processed_video.setField<int>("id", processed_video_id);
		//processed_video.readFromDB();
		// Select all detections for that video and that software component
		PerlaGtBestObject query_object;
		query_object.setField<int>("best_ground_truth_id", ground_truth_id);
		vector<PerlaGtBestObject*> objects = connection->queryList<PerlaGtBestObject>(query_object, vector<string>(), "ORDER BY frame_number");
		// Add each detection
		int counter = 1;
		for(vector<PerlaGtBestObject*>::iterator o_it = objects.begin(); o_it != objects.end(); ++o_it)
		{
			try
			{
				Log::debug(0) << "Reading object " << counter++ << endl;
				// Get object
				PerlaGtBestObject& gt_object = **o_it;
				// Create object
				pr::Object object;
				// Set data
				object.setId(gt_object.getField<int>("object_id"));
				object.setFrameNumber(gt_object.getField<int>("frame_number"));
				object.setType("fish");
				vector<Point> contour = gt_object.getField<vector<Point> >("contour");
				object.setContour(contour);
				// Add object
				results.addObject(object);
			}
			catch(MyException& e)
			{
				Log::w() << "Error reading object, skipping: " << e.what() << endl;
				continue;
			}
			catch(...)
			{
				Log::w() << "Error reading object, skipping." << endl;
				continue;
			}
		}
		// Return results
		return results;
	}
	
	// Get results from ground_truth/gt_object tables
	pr::Results DBHelper::getPerlaGTResults(DBConnection* connection, int ground_truth_id)
	{
		// Define output
		pr::Results results;
		// Get processed video data
		//ProcessedVideo processed_video(connection);
		//processed_video.setField<int>("id", processed_video_id);
		//processed_video.readFromDB();
		// Select all detections for that video and that software component
		PerlaGtObject query_object;
		query_object.setField<int>("ground_truth_id", ground_truth_id);
		vector<PerlaGtObject*> objects = connection->queryList<PerlaGtObject>(query_object, vector<string>(), "ORDER BY frame_number");
		// Add each detection
		int counter = 1;
		for(vector<PerlaGtObject*>::iterator o_it = objects.begin(); o_it != objects.end(); ++o_it)
		{
			try
			{
				Log::debug(0) << "Reading object " << counter++ << endl;
				// Get object
				PerlaGtObject& gt_object = **o_it;
				// Create object
				pr::Object object;
				// Set data
				object.setId(gt_object.getField<int>("object_id"));
				object.setFrameNumber(gt_object.getField<int>("frame_number"));
				object.setType("fish");
				vector<Point> contour = gt_object.getField<vector<Point> >("contour");
				object.setContour(contour);
				// Add object
				results.addObject(object);
			}
			catch(MyException& e)
			{
				Log::w() << "Error reading object, skipping: " << e.what() << endl;
				continue;
			}
			catch(...)
			{
				Log::w() << "Error reading object, skipping." << endl;
				continue;
			}
		}
		// Return results
		return results;
	}
	
	// Get results from ground_truth/gt_object tables
	pr::Results DBHelper::getGTResults(DBConnection* connection, int ground_truth_id)
	{
		// Define output
		pr::Results results;
		// Select keyframes
		GtKeyFrames query_keyframes;
		query_keyframes.setField<int>("ground_truth_id", ground_truth_id);
		vector<GtKeyFrames*> keyframes = connection->queryList<GtKeyFrames>(query_keyframes, vector<string>());
		// Add keyframes
		for(vector<GtKeyFrames*>::iterator k_it = keyframes.begin(); k_it != keyframes.end(); k_it++)
		{
			// Get keyframes
			GtKeyFrames& kf = **k_it;
			// Get data
			int start = kf.getField<int>("start");
			int end = kf.getField<int>("end");
			string label = kf.getField<string>("label");
			// Add to results
			results.getKeyFrames().add(start, end, label);
		}
		// Select all detections for that video and that software component
		GtObject query_object;
		query_object.setField<int>("ground_truth_id", ground_truth_id);
		vector<GtObject*> objects = connection->queryList<GtObject>(query_object, vector<string>(), "ORDER BY frame_number");
		// Add each detection
		int counter = 1;
		for(vector<GtObject*>::iterator o_it = objects.begin(); o_it != objects.end(); ++o_it)
		{
			try
			{
				Log::debug(0) << "Reading object " << counter++ << endl;
				// Get object
				GtObject& gt_object = **o_it;
				// Create object
				pr::Object object;
				// Set data
				object.setId(gt_object.getField<int>("object_id"));
				object.setFrameNumber(gt_object.getField<int>("frame_number"));
				object.setType(gt_object.getField<string>("type"));
				object.setOptional(gt_object.getField<bool>("optional"));
				vector<Point> contour = gt_object.getField<vector<Point> >("contour");
				object.setContour(contour);
				// Add object
				results.addObject(object);
			}
			catch(MyException& e)
			{
				Log::w() << "Error reading object, skipping: " << e.what() << endl;
				continue;
			}
			catch(...)
			{
				Log::w() << "Error reading object, skipping." << endl;
				continue;
			}
		}
		// Free data
		for(vector<GtKeyFrames*>::iterator it = keyframes.begin(); it != keyframes.end(); it++)		delete *it;
		for(vector<GtObject*>::iterator it = objects.begin(); it != objects.end(); it++)		delete *it;
		// Return results
		return results;
	}
}
