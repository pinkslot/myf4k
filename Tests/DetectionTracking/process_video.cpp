// Main executable for the detection and tracking of fish from an input video.
// Common includes
#include <iostream>
#include <iomanip>
#include <cv.h>
#include <time_utils.h>
#include <math_utils.h>
#include <shell_utils.h>
#include <results_utils.h>
#include <opencv_utils.h>
#include <machine_utils.h>
#include <detection_summary.h>
#include <cvblob.h>
#include <cstring>
#include <tclap/CmdLine.h>
#include <log.h>
#include <results.h>
#include <xmltdresults.h>
#include <xmlconfig.h>
#include <csignal>
#include <unistd.h>
#include <context.h>
// Database includes
#include <processed_video.h>
#include <video.h>
#include <feature.h>
#include <video_feature.h>
#include <fish.h>
#include <fish_detection.h>
#include <software_component.h>
#include <machine.h>
#include <contour_encoder.h>
// Algorithm includes
#include <preprocessing_chooser.h>
#include <detection_chooser.h>
#include <postprocessing_chooser.h>
#include <blob_processing_chooser.h>
#include <tracker_chooser.h>
#include <result_processing_chooser.h>
#include <detection_evaluation_chooser.h>
#include <tracking_evaluation_chooser.h>
#include <bayes_detection_evaluator.h>

using namespace cv;
using namespace pr;
using namespace std;
using namespace TCLAP;
using namespace alg;

void addMargin(cvb::CvBlob& blob, unsigned int margin, unsigned int height, unsigned int width);
	
// Define database connection and processed video objects - out here, for segmentation fault handling
db::DBConnection *db_connection = NULL;
bool enable_db = false;
bool processed_video_exists = false;
db::ProcessedVideo processed_video;
int frames_with_errors = 0;

// Update video status, if connected to database
void updateVideoStatus(string status, bool lock = false)
{
	if(enable_db && db_connection != NULL && processed_video_exists)
	{
		try
		{
			/*if(lock)
			{
				// Lock tables
				db_connection->lockTables("processed_videos WRITE");
			}*/
			processed_video.setField<string>("status", status);
			processed_video.setField<int>("frames_with_errors", frames_with_errors);
			processed_video.writeToDB();
			/*if(lock)
			{
				// Unlock tables
				db_connection->unlockTables();
			}*/
		}
		catch(MyException& e)
		{
			Log::error(0) << "Cannot save video processing status to database: " << e.what() << endl;
			exit(1);
		}
		catch(...)
		{
			Log::error(0) << "Cannot save video processing status to database." << endl;
			exit(1);
		}
	}
}

// Update video progress, if connected to database
void updateVideoProgress(int progress, bool lock = false)
{
	if(enable_db && db_connection != NULL && processed_video_exists)
	{
		try
		{
			/*if(lock)
			{
				// Lock tables
				db_connection->lockTables("processed_videos WRITE");
			}*/
			processed_video.setField<int>("progress", progress);
			processed_video.writeToDB();
			/*if(lock)
			{
				// Unlock tables
				db_connection->unlockTables();
			}*/
		}
		catch(MyException& e)
		{
			Log::error(0) << "Cannot save video progress to database: " << e.what() << endl;
			exit(1);
		}
		catch(...)
		{
			Log::error(0) << "Cannot save video progress status to database." << endl;
			exit(1);
		}
	}
}

// Error handler
void errorSignalHandler(int sig)
{
	// Prevent infinite recursion
	signal(sig, SIG_DFL);
	Log::error(0) << "Caught error signal: " << strsignal(sig) << endl;
	// Try to mark the video in an error status
	if(enable_db)
	{
		Log::error(0) << "Attempting to save video state." << endl;
		updateVideoStatus("error", false);
	}
	exit(1);
}

// Initialize context
Context C;
// Define buffer for each frame
Mat draw_frame;
vector<const TrackedObject *> cur_tracked_objects;
string video_path;

void on_mouse(int evt, int x, int y, int flags, void* param)
{
	string src_prefix("GroundTruth/src/"), mask_prefix("GroundTruth/mask/");

    if(evt==CV_EVENT_LBUTTONDOWN)
    {
		Log::info(0) << "x: " << x << "y " << y <<endl;
		for (unsigned int i = 0; i < cur_tracked_objects.size(); i++)
		{
			cvb::CvContourPolygon *contour = cvb::cvConvertChainCodesToPolygon(&cur_tracked_objects[i]->currentRegion().contour);				
			vector<cv::Point>  pcontour(contour->size());
			for (unsigned j =0; j < contour->size(); j++)
				pcontour[j] = cv::Point((*contour)[j]);
			if (contour->size() > 3 && cv::pointPolygonTest(pcontour, Point2f(x, y), false) > 0)
			{
				Log::info(0) << "contour no: " << i <<endl;
				string name(video_path + boost::to_string(C.frame_num) + boost::to_string(i) + ".bmp");
				Mat mask = Mat::zeros(draw_frame.size(), CV_8UC1);
				for (int y = 0; y < draw_frame.rows; y++)
					for (int x = 0; x < draw_frame.cols; x++)
						if (cv::pointPolygonTest(pcontour, Point2f(x, y), false) > 0)
							mask.at<uchar>(y, x, 0) = 255;

				imwrite(src_prefix + name, C.frame);
				imwrite(mask_prefix + name, mask);				
				return;
			}	
		}
		Log::info(0) << "miss contour" <<endl;
	}
}

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

struct stat st = {0};

int main(int argc, char** argv)
{

	if (stat("GroundTruth", &st) == -1)
    	mkdir("GroundTruth", 0700);
	if (stat("GroundTruth/src/", &st) == -1)
    	mkdir("GroundTruth/src/", 0700);    
	if (stat("GroundTruth/mask/", &st) == -1)
    	mkdir("GroundTruth/mask/", 0700);  


    VideoWriter writer("result.avi", CV_FOURCC('M','J','P','G'), 8, cvSize(640, 480));	
	// Set db connection tracking data
	db::DBConnection::setCmd(argc, argv);
	// Setup signal handlers
	signal(SIGFPE, errorSignalHandler);
	signal(SIGKILL, errorSignalHandler);
	//signal(SIGSEGV, errorSignalHandler);
	signal(SIGABRT, errorSignalHandler);
	signal(SIGINT, errorSignalHandler);
	signal(SIGQUIT, errorSignalHandler);
	signal(SIGTERM, errorSignalHandler);
	try
	{
		// Define variables for command-line parameters
		vector<string> frame_preproc_alg;
		string detection_alg;
		vector<string> post_processing_alg;
		vector<string> blob_filtering_alg;
		string tracking_alg;
		string detection_evaluation_alg;
		string tracking_evaluation_alg;
		vector<string> result_processing_alg;
		vector<string> frame_preproc_settings;
		string detection_settings;
		vector<string> blob_filtering_settings;
		vector<string> post_processing_settings;
		string tracking_settings;
		string detection_evaluation_settings;
		string tracking_evaluation_settings;
		vector<string> result_processing_settings;
		string xml_config_path;
		string video_id;
		int debug_level;
		int debug_from;
		int debug_to;
		bool enable_debug;
		int process_from;
		int process_to;
		string db_config_path;
		string output;
		string xml_path;
		bool enable_summary_images;
		string summary_prefix;
		string certainty_output;
		//unsigned int minimum_appearances;
		bool no_wait;
		bool save_video_preview;
		bool print_component_id;
		bool no_db_results;
		string gt_path;
		bool show_motion_output;
		bool show_postproc_output;
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
			// Frame pre-processing argument
			vector<string> frame_preproc_algs = PreProcessingChooser::list();
			ValuesConstraint<string> frame_preproc_constraint(frame_preproc_algs);
			MultiArg<string> frame_preproc_alg_arg("", "frame_preproc", "Frame pre-processing algorithm", false, &frame_preproc_constraint, cmd);
			// Define detection argument
			vector<string> detection_algs = DetectionChooser::list();
			ValuesConstraint<string> detection_constraint(detection_algs);
			ValueArg<string> detection_alg_arg("", "detection", "Detection algorithm short name", false, "gmm",  &detection_constraint);
			cmd.add(detection_alg_arg);
			// Post-processing
			vector<string> post_processing_algs = PostProcessingChooser::list();
			ValuesConstraint<string> post_processing_constraint(post_processing_algs);
			MultiArg<string> post_processing_alg_arg("", "post_processing", "Post-processing algorithm", false, &post_processing_constraint, cmd);
			// Blob filtering
			vector<string> blob_filtering_algs = BlobProcessingChooser::list();
			ValuesConstraint<string> blob_filtering_constraint(blob_filtering_algs);
			MultiArg<string> blob_filtering_alg_arg("", "blob_filtering", "Blob filtering algorithm", false, &blob_filtering_constraint, cmd);
			// Tracking
			vector<string> tracking_algs = TrackerChooser::list();
			ValuesConstraint<string> tracking_constraint(tracking_algs);
			ValueArg<string> tracking_alg_arg("", "tracking", "Tracking algorithm short name", false, "cov",  &tracking_constraint);
			cmd.add(tracking_alg_arg);
			// Detection evaluation
			vector<string> detection_evaluation_algs = DetectionEvaluationChooser::list();
			ValuesConstraint<string> detection_evaluation_constraint(detection_evaluation_algs);
			ValueArg<string> detection_evaluation_alg_arg("", "detection_evaluation", "Detection evaluation algorithm", false, "",  &detection_evaluation_constraint);
			cmd.add(detection_evaluation_alg_arg);
			// Tracking evaluation
			vector<string> tracking_evaluation_algs = TrackingEvaluationChooser::list();
			ValuesConstraint<string> tracking_evaluation_constraint(tracking_evaluation_algs);
			ValueArg<string> tracking_evaluation_alg_arg("", "tracking_evaluation", "Tracking evaluation algorithm", false, "",  &tracking_evaluation_constraint);
			cmd.add(tracking_evaluation_alg_arg);
			// Result processing
			vector<string> result_processing_algs = ResultProcessingChooser::list();
			ValuesConstraint<string> result_processing_constraint(result_processing_algs);
			MultiArg<string> result_processing_alg_arg("", "result_processing", "Result processing algorithm", false, &result_processing_constraint, cmd);
			// Frame pre-processing settings from command-line
			MultiArg<string> frame_preproc_settings_arg("", "frame_preproc-settings", "Frame pre-processing settings (WARNING: the i-th frame_preproc-settings argument is associated to the i-th frame_preproc argument)", false, "k1=v1,k2=v2,kN=vN", cmd);
			// Detection settings from command-line
			ValueArg<string> detection_settings_arg("", "detection-settings", "Settings string for detection.", false, "", "k1=v1,k2=v2,...,kN=vN", cmd);
			// Post-processing settings from command-line
			MultiArg<string> post_processing_settings_arg("", "post_processing-settings", "Post-processing settings (WARNING: the i-th post_processing-settings argument is associated to the i-th post_processing argument)", false, "k1=v1,k2=v2,kN=vN", cmd);
			// Blob filtering settings from command-line
			MultiArg<string> blob_filtering_settings_arg("", "blob_filtering-settings", "Blob filtering settings (WARNING: the i-th blob_filtering-settings argument is associated to the i-th blob_filtering argument)", false, "k1=v1,k2=v2,kN=vN", cmd);
			// Tracking settings from command-line
			ValueArg<string> tracking_settings_arg("", "tracking-settings", "Tracking settings.", false, "", "k1=v1,k2=v2,...,kN=vN", cmd);
			// Detection evaluation settings from command-line
			ValueArg<string> detection_evaluation_settings_arg("", "detection_evaluation-settings", "Settings string for detection evaluation.", false, "", "k1=v1,k2=v2,...,kN=vN", cmd);
			// Tracking evaluation settings from command-line
			ValueArg<string> tracking_evaluation_settings_arg("", "tracking_evaluation-settings", "Settings string for tracking evaluation.", false, "", "k1=v1,k2=v2,...,kN=vN", cmd);
			// Result processing settings from command-line
			MultiArg<string> result_processing_settings_arg("", "result_processing-settings", "Result processing settings (WARNING: the i-th result_processing-settings argument is associated to the i-th result_processing argument)", false, "k1=v1,k2=v2,kN=vN", cmd);
			// XML configuration
			ValueArg<string> xml_config_path_arg("", "sw-config", "Path to XML configuration file.", false, "", "path", cmd);
			// Video path - XORed with print_component_id
			ValueArg<string> video_path_arg("v", "video", "Input video path", true, "video.mpg", "path");
			// Print component id
			SwitchArg print_component_id_arg("", "print-component-id", "Print component ID and exit", false);
			cmd.xorAdd(video_path_arg, print_component_id_arg);
			// Video ID
			ValueArg<string> video_id_arg("i", "video-id", "Video database ID", false, "-1", "string", cmd);
			// Define debug argument
			MultiSwitchArg debug_arg("d", "debug", "Print/show debug information (-d: show interesting frames only; -dd: show all frames)", cmd);
			// Define debug frame limits 
			ValueArg<int> debug_from_arg("", "debug-from", "Show debug window starting from given frame.", false, 0, "int", cmd);
			ValueArg<int> debug_to_arg("", "debug-to", "Show debug window until given frame.", false, INT_MAX, "int", cmd);
			// Define processing frame limits 
			ValueArg<int> process_from_arg("", "process-from", "Process from given frame.", false, 0, "int", cmd);
			ValueArg<int> process_to_arg("", "process-to", "Process until given frame.", false, INT_MAX, "int", cmd);
			// Define database enable argument
			SwitchArg enable_db_arg("", "db", "Enable writing results to database", cmd, false);
			// Database configuration file path
			ValueArg<string> db_config_path_arg("", "db-config", "Path to database connection configuration file", false, "db_config.xml", "path", cmd);
			// Define output destination argument
			vector<string> output_strings;
			output_strings.push_back("file");
			output_strings.push_back("stdout");
			output_strings.push_back("none");
			ValuesConstraint<string> output_constraint(output_strings);
			ValueArg<string> output_arg("", "output", "Output destination", false, "stdout", &output_constraint);
			cmd.add(output_arg);
			// Define XML result output file
			ValueArg<string> xml_path_arg("x", "xml", "XML result output file path", false, "results.xml", "path", cmd);
			// Enable summary images
			SwitchArg enable_summary_images_arg("", "summary", "Enable generation of detection summary images.", cmd, false);
			// Define prefix for summary images
			ValueArg<string> summary_prefix_arg("s", "summary-prefix", "Prefix for summary images path (if it is a directory, add trailing slash)", false, "", "path", cmd);
			// Certainty output file
			ValueArg<string> certainty_output_arg("c", "certainty-out", "Certainty output file path.", false, "", "path", cmd);
			// Minimum appearances for object
			//ValueArg<unsigned int> minimum_appearances_arg("", "min-appears", "Minimum number of appearances needed to save an object.", false, 2, "int", cmd);
			// No-wait flag
			SwitchArg no_wait_arg("n", "no-wait", "Skip waiting time before processing.", cmd, false);
			// Save video preview imageВОШЬ ДРОЖАЩАЯ
			SwitchArg save_video_preview_arg("", "save-preview", "Save video preview image to preview.png.", cmd, false);
			// Dummy argument to be used with grep, to distinguish instances of process_video
			ValueArg<string> tag_arg("", "tag", "Process tag (for use with grep).", false, "", "string", cmd);
			// Don't save results to database
			SwitchArg no_db_results_arg("", "no-db-results", "Don't save results to database (doesn't disable other database features, e.g. processed videos).", cmd, false);
			// Show ground truth
			ValueArg<string> gt_path_arg("g", "gt", "Path to ground truth XML file", false, "", "path", cmd);
			// Show several frame processing levels, if debug is enabled
			SwitchArg show_motion_output_arg("", "show-motion", "Show motion mask", cmd, false);
			SwitchArg show_postproc_output_arg("", "show-postproc", "Show post-processing output", cmd, false);
			// Parse actual arguments
			cmd.parse(argc, argv);
			// Read easy values
			frame_preproc_alg = frame_preproc_alg_arg.getValue();
			detection_alg = detection_alg_arg.getValue();
			post_processing_alg = post_processing_alg_arg.getValue();
			blob_filtering_alg = blob_filtering_alg_arg.getValue();
			tracking_alg = tracking_alg_arg.getValue();
			detection_evaluation_alg = detection_evaluation_alg_arg.getValue();
			tracking_evaluation_alg = tracking_evaluation_alg_arg.getValue();
			result_processing_alg = result_processing_alg_arg.getValue();
			frame_preproc_settings = frame_preproc_settings_arg.getValue();
			detection_settings = detection_settings_arg.getValue();
			blob_filtering_settings = blob_filtering_settings_arg.getValue();
			post_processing_settings = post_processing_settings_arg.getValue();
			tracking_settings = tracking_settings_arg.getValue();
			detection_evaluation_settings = detection_evaluation_settings_arg.getValue();
			tracking_evaluation_settings = tracking_evaluation_settings_arg.getValue();
			result_processing_settings = result_processing_settings_arg.getValue();
			xml_config_path = xml_config_path_arg.getValue();
			video_path = video_path_arg.getValue();
			print_component_id = print_component_id_arg.getValue();
			video_id = video_id_arg.getValue();
			debug_level = debug_arg.getValue();
			debug_from = debug_from_arg.getValue();
			debug_to = debug_to_arg.getValue();
			enable_debug = debug_level > 0 || debug_from_arg.isSet() || debug_to_arg.isSet();
			process_from = process_from_arg.getValue();
			process_to = process_to_arg.getValue();
			enable_db = enable_db_arg.getValue();
			db_config_path = db_config_path_arg.getValue();
			output = output_arg.getValue();
			xml_path = xml_path_arg.getValue();
			enable_summary_images = enable_summary_images_arg.getValue();
			summary_prefix = summary_prefix_arg.getValue();
			certainty_output = certainty_output_arg.getValue();
			//minimum_appearances = minimum_appearances_arg.getValue();
			no_wait = no_wait_arg.getValue();
			save_video_preview = save_video_preview_arg.getValue();
			no_db_results = no_db_results_arg.getValue();
			gt_path = gt_path_arg.getValue();
			show_motion_output = show_motion_output_arg.getValue();
			show_postproc_output = show_postproc_output_arg.getValue();
			// If we have to print the component id, force enable_db to true




			if(print_component_id)
			{
				enable_db = true;
			}
			// Check dependencies for complicated stuff
			// Check that at least the detection/tracking algorithms or the XML config file have been provided
			if(!(detection_alg_arg.isSet() && tracking_alg_arg.isSet()) && !xml_config_path_arg.isSet())
			{
				ArgException exc("At least the detection/tracking algorithms or the XML configuration file must be provided");
				cmd.getOutput()->failure(cmd, exc);
			}
			// If the XML config file has been provided, check no other component-related arguments have
			if(xml_config_path_arg.isSet() && (frame_preproc_alg_arg.isSet() || frame_preproc_settings_arg.isSet() || detection_alg_arg.isSet() || detection_settings_arg.isSet() || blob_filtering_alg_arg.isSet() || blob_filtering_settings_arg.isSet() || tracking_alg_arg.isSet() || tracking_settings_arg.isSet() || detection_evaluation_alg_arg.isSet() || detection_evaluation_settings_arg.isSet() || tracking_evaluation_alg_arg.isSet() || tracking_evaluation_settings_arg.isSet()))
			{
				ArgException exc("If the XML configuration file is provided, no algorithm-related arguments can.");
				cmd.getOutput()->failure(cmd, exc);
			}
			// If we have to write to the database, check we have the video information
			if(enable_db && !video_id_arg.isSet() && !print_component_id)//|| ! datetimeArg.isSet()))
			{
				ArgException exc("Missing video-id (necessary for the database interface)");
				cmd.getOutput()->failure(cmd, exc);
			}
			if(tracking_alg == "xml" && detection_alg != "xml")
			{
				ArgException exc("Tracking based on XML file must be associated to XML detection.");
				cmd.getOutput()->failure(cmd, exc);
			}
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
		// Set log output
		if(output == "file")
		{
			// FIXME
			/*Log::setDebugLogFile("debug.log");
			Log::setInfoLogFile("info.log");
			Log::setWarningLogFile("warning.log");
			Log::setErrorLogFile("error.log");*/
		}
		else if(output == "stdout")
		{
			if(enable_debug)
			{
				Log::setDebugLogStream(cout);
			}
			Log::setInfoLogStream(cout);
			Log::setWarningLogStream(cout);
			Log::setErrorLogStream(cerr);
		}
		else
		{
			ostream* sink = new ostream(0);
			Log::setDebugLogStream(*sink);
			Log::setInfoLogStream(*sink);
			Log::setWarningLogStream(*sink);
			Log::setErrorLogStream(*sink);
		}
		// ------ Component definition ------
		// Check XML configuration path
		if(xml_config_path.size() > 0)
		{
			// Read XML file
			XMLConfig config;
			config.readFile(xml_config_path);
			// Set algorithms and settings
			frame_preproc_alg = config.getPreProcessing();
			frame_preproc_settings = config.getPreProcessingSettings();
			detection_alg = config.getDetection();
			detection_settings = config.getDetectionSettings();
			post_processing_alg = config.getPostProcessing();
			post_processing_settings = config.getPostProcessingSettings();
			blob_filtering_alg = config.getBlobProcessing();
			blob_filtering_settings = config.getBlobProcessingSettings();
			tracking_alg = config.getTracking();
			tracking_settings = config.getTrackingSettings();
			detection_evaluation_alg = config.getDetectionEvaluation();
			detection_evaluation_settings = config.getDetectionEvaluationSettings();
			tracking_evaluation_alg = config.getTrackingEvaluation();
			tracking_evaluation_settings = config.getTrackingEvaluationSettings();
			result_processing_alg = config.getResultProcessing();
			result_processing_settings = config.getResultProcessingSettings();
		}
		// Create pre-processing module
		PreProcessing* pre_processing = new PreProcessing();
		for(unsigned int i=0; i<frame_preproc_alg.size(); ++i)
		{
			// Create algorithm instance
			PreProcessing* sub_filter = PreProcessingChooser::create(frame_preproc_alg[i]);
			// Set parameters, if provided
			if(frame_preproc_settings.size() > i)
			{
				sub_filter->getParameters().readFromString(frame_preproc_settings[i]);
			}
			// Add sub filter
			pre_processing->addSubFilter(sub_filter);
		}
		// Set detection algorithm
		Detection *detection = DetectionChooser::create(detection_alg);
		detection->getParameters().readFromString(detection_settings);
		// Create post-processing module
		PostProcessing* post_processing = new PostProcessing();
		for(unsigned int i=0; i<post_processing_alg.size(); ++i)
		{
			// Create algorithm instance
			PostProcessing* sub_filter = PostProcessingChooser::create(post_processing_alg[i]);
			// Set parameters, if provided
			if(post_processing_settings.size() > i)
			{
				sub_filter->getParameters().readFromString(post_processing_settings[i]);
			}
			// Add sub filter
			post_processing->addSubFilter(sub_filter);
		}
		// Create blob processing module
		BlobProcessing* blob_processing = new BlobProcessing();
		// Flag to check if a blob size filter has been introduced
		//bool blob_size_filter_present = false;
		for(unsigned int i=0; i<blob_filtering_alg.size(); ++i)
		{
			// Create algorithm instance
			//if(blob_filtering_alg[i] == "blob_size_filter")
			//	blob_size_filter_present = true;
			BlobProcessing* sub_filter = BlobProcessingChooser::create(blob_filtering_alg[i]);
			// Set parameters, if provided
			if(blob_filtering_settings.size() > i)
			{
				sub_filter->getParameters().readFromString(blob_filtering_settings[i]);
			}
			// Add sub filter
			blob_processing->addSubFilter(sub_filter);
		}
		/*if(!blob_size_filter_present)
		{
			// Add by default a size filter to remove little blobs
			BlobProcessing* default_blob_filter = BlobProcessingChooser::create("blob_size_filter");
			default_blob_filter->getParameters().set<float>("area_min_per", 0.003);
			default_blob_filter->getParameters().set<float>("area_max_per", 1.5);
			blob_processing->addSubFilter(default_blob_filter);
		}*/
		// Set tracking algorithm
		Tracker *tracker = TrackerChooser::create(tracking_alg);
		tracker->getParameters().readFromString(tracking_settings);
		// If tracker is dummy tracker, set minimum appearances to 1
		//if(tracking_alg == "dummy")
		//{
		//	minimum_appearances = 1;
		//}
		// Create result-processing module
		ResultProcessing* result_processing = new ResultProcessing();
		for(unsigned int i=0; i<result_processing_alg.size(); ++i)
		{
			// Create algorithm instance
			ResultProcessing* sub_filter = ResultProcessingChooser::create(result_processing_alg[i]);
			// Set parameters, if provided
			if(result_processing_settings.size() > i)
			{
				sub_filter->getParameters().readFromString(result_processing_settings[i]);
			}
			// Add sub filter
			result_processing->addSubFilter(sub_filter);
		}
		// Create detection evaluator
		Evaluator *detection_evaluator = NULL;
		try
		{
			detection_evaluator = DetectionEvaluationChooser::create(detection_evaluation_alg);
			detection_evaluator->getParameters().readFromString(detection_evaluation_settings);
		}
		catch(MyException& e)
		{
			Log::info(0) << "Caught: " << e.what() << " (probably means no detection evaluation algorithm was specified)." << endl;
		}
		catch(...)
		{
			Log::info(0) << "Caught exception (probably means no detection evaluation algorithm was specified)." << endl;
		}
		// Create detection evaluator
		Evaluator* tracking_evaluator = NULL;
		try
		{
			tracking_evaluator = TrackingEvaluationChooser::create(tracking_evaluation_alg);
			tracking_evaluator->getParameters().readFromString(tracking_evaluation_settings);
		}
		catch(MyException& e)
		{
			Log::info(0) << "Caught: " << e.what() << " (probably means no tracking evaluation algorithm was specified)." << endl;
		}
		catch(...)
		{
			Log::info(0) << "Caught exception (probably means no tracking evaluation algorithm was specified)." << endl;
		}
		// Check video_id is valid
		if(enable_db && video_id == "-1" && !print_component_id)
		{
			Log::error() << "Invalid video ID." << endl;
			exit(1);
		}
		// Open the video file
		VideoCapture video_cap(video_path);
		// Check for errors
		if(!video_cap.isOpened() && !print_component_id)
		{
			Log::error() << "Error opening the video file." << endl;
			updateVideoStatus("error");
			exit(1);
		}
		// Define software component
		db::SoftwareComponent *software_component = new db::SoftwareComponent();
		int component_id = -1;
		// Initialize max number of frames to compute progress (only if db connection is enabled)
		int total_frames = -1;
		if(enable_db)
		{
			try
			{
				// Try to connect to database
				ifstream db_config_file(db_config_path.c_str());
				if(db_config_file.good())
				{
					Log::info(0) << "Using database configuration file: " << db_config_path << endl;
					db_connection = new db::DBConnection(db_config_path);
				}
				else
				{
					Log::e() << "Error reading database configuration file." << endl;
					exit(1);
				}
				// Lock tables
				//db_connection->lockTables("software_component WRITE, algorithm WRITE");
				// Define software component
				software_component->setConnection(db_connection);
				// Add pre-processing algorithms
				vector<alg::Algorithm*> pre_proc_algs = pre_processing->getSubFiltersAsAlgs();
				for(vector<alg::Algorithm*>::iterator it = pre_proc_algs.begin(); it != pre_proc_algs.end(); it++)
					software_component->addAlgorithm(*it);
				// Add detection algorithm
				software_component->addAlgorithm(detection);
				// Add post-processing algorithms
				vector<alg::Algorithm*> post_proc_algs = post_processing->getSubFiltersAsAlgs();
				for(vector<alg::Algorithm*>::iterator it = post_proc_algs.begin(); it != post_proc_algs.end(); it++)
					software_component->addAlgorithm(*it);
				// Add blob processing algorithms
				vector<alg::Algorithm*> blob_proc_algs = blob_processing->getSubFiltersAsAlgs();
				for(vector<alg::Algorithm*>::iterator it = blob_proc_algs.begin(); it != blob_proc_algs.end(); it++)
					software_component->addAlgorithm(*it);
				// Add tracker
				software_component->addAlgorithm(tracker);
				// Add result processing algorithms
				vector<alg::Algorithm*> res_proc_algs = result_processing->getSubFiltersAsAlgs();
				for(vector<alg::Algorithm*>::iterator it = res_proc_algs.begin(); it != res_proc_algs.end(); it++)
					software_component->addAlgorithm(*it);
				// Add detection evaluator
				if(detection_evaluator != NULL)
				{
					software_component->addAlgorithm(detection_evaluator);
				}
				// Add tracking evaluator
				if(tracking_evaluator != NULL)
				{
					software_component->addAlgorithm(tracking_evaluator);
				}
				// Check if component exists
				bool component_exists = software_component->lookup();
				if(!component_exists)
				{
					// Insert component data
					software_component->insertToDB();
				}
				component_id = software_component->getField<int>("id");
				if(print_component_id)
				{
					cout << "COMPONENT_ID " << component_id << endl;
					exit(0);
				}
				Log::d() << "Component id: " << component_id << endl;
				// Unlock tables
				//db_connection->unlockTables();
				// Lock tables
				//Log::d() << "Locking tables for checking previous processing." << endl;
				//db_connection->lockTables("processed_videos WRITE, video READ, machine WRITE, fish WRITE, fish_detection WRITE");
				//Log::d() << "Locked." << endl;
				// Read total frames
				db::Video video(db_connection);
				video.setField<string>("video_id", video_id);
				video.readFromDB();
				total_frames = video.getField<int>("length");
				Log::info(0) << "Total frames: " << total_frames << endl;
				// Check processed video
				processed_video.setConnection(db_connection);
				processed_video.setField<string>("video_id", video_id);
				processed_video.setField<int>("component_id", component_id);
				Log::info(0) << "Looking for previous processing results..." << endl;
				processed_video_exists = processed_video.lookup();
				if(!processed_video_exists)
				{
					// Read machine information
					db::Machine machine(db_connection);
					machine.readFromSystem();
					// Lookup and add machine, if necessary
					if(!machine.lookup())
					{
						machine.insertToDB();
					}
					processed_video.setField<int>("machine_id", machine.getField<int>("id"));
					Log::info(0) << "Inserting new processed video record." << endl;
					// Insert into database
					processed_video.insertToDB();
					// Mark as existing
					processed_video_exists = true;
					// Set status
					updateVideoStatus("starting", false);
				}
				else
				{
					// Get processed video data
					processed_video.readFromDB();
					// Get status
					string status = processed_video.getField<string>("status");
					if(status == "error")
					{
						Log::info(0) << "Video processing status is 'error', reprocessing." << endl;
						// Clear previous results
						processed_video.clearPreviousResults();
						// Restart
						updateVideoStatus("restarting", false);
					}
					else if(status == "pending")
					{
						Log::info(0) << "Video processing status is 'pending', starting." << endl;
						// Clear previous results
						processed_video.clearPreviousResults();
						// Restart
						updateVideoStatus("starting", false);
					}
					else
					{
						Log::error(0) << "Video already processed/being processed with this software component, exiting." << endl;
						return 1;
					}
				}
				// Unlock tables
				//db_connection->unlockTables();
			}
			catch(MyException& e)
			{
				Log::error(0) << "Database error: " << e.what() << endl;
				// Unlock tables
				//db_connection->unlockTables();
				// Try to update video status
				updateVideoStatus("error");
				return 1;
			}
			catch(...)
			{
				Log::error(0) << "Database error." << endl;
				// Unlock tables
				//db_connection->unlockTables();
				// Try to update video status
				updateVideoStatus("error");
				return 1;
			}
		}
		// Check for results from previous processing
		if(enable_db)
		{
			// Lock tables
			//db_connection->lockTables("processed_videos WRITE, fish WRITE, fish_detection WRITE");
			if(processed_video.hasPreviousResults())
			{
				Log::warning() << "Found unexpected detections for this video/component pair (" << video_id << "/" << component_id << "), deleting them." << endl;
				// Clear previous results
				processed_video.clearPreviousResults();
			}
			// Unlock tables
			//db_connection->unlockTables();
		}
		// Print parameters
		// Pre-processing
		vector<alg::Algorithm*> pre_proc_algs = pre_processing->getSubFiltersAsAlgs();
		for(vector<alg::Algorithm*>::iterator it = pre_proc_algs.begin(); it != pre_proc_algs.end(); it++)
		{
			Log::info(0) << "Preprocessing: " << (*it)->name() << endl;
			vector<string> params = (*it)->getParameters().list();
			for(vector<string>::iterator p_it = params.begin(); p_it != params.end(); p_it++)
				Log::info(0) << "\t" << *p_it << ": " << (*it)->getParameters().get<string>(*p_it) << endl;
		}
		// Detection
		Log::info(0) << "Detection algorithm: " << detection_alg << endl;
		vector<string> detection_params = detection->getParameters().list();
		for(vector<string>::iterator it = detection_params.begin(); it != detection_params.end(); it++)
		{
			Log::info(0) << "\t" << *it << ": " << detection->getParameters().get<string>(*it) << endl;
		}
		// Post-processing
		vector<alg::Algorithm*> post_proc_algs = post_processing->getSubFiltersAsAlgs();
		for(vector<alg::Algorithm*>::iterator it = post_proc_algs.begin(); it != post_proc_algs.end(); it++)
		{
			Log::info(0) << "Post-processing: " << (*it)->name() << endl;
			vector<string> params = (*it)->getParameters().list();
			for(vector<string>::iterator p_it = params.begin(); p_it != params.end(); p_it++)
				Log::info(0) << "\t" << *p_it << ": " << (*it)->getParameters().get<string>(*p_it) << endl;
		}
		// Blob processing
		vector<alg::Algorithm*> blob_proc_algs = blob_processing->getSubFiltersAsAlgs();
		for(vector<alg::Algorithm*>::iterator it = blob_proc_algs.begin(); it != blob_proc_algs.end(); it++)
		{
			Log::info(0) << "Blob processing: " << (*it)->name() << endl;
			vector<string> params = (*it)->getParameters().list();
			for(vector<string>::iterator p_it = params.begin(); p_it != params.end(); p_it++)
			{
				Log::info(0) << "\t" << *p_it << ": " << (*it)->getParameters().get<string>(*p_it) << endl;
			}
		}
		// Tracking
		Log::info(0) << "Tracking algorithm: " << tracking_alg << endl;
		vector<string> tracking_params = tracker->getParameters().list();
		for(vector<string>::iterator it = tracking_params.begin(); it != tracking_params.end(); it++)
		{
			Log::info(0) << "\t" << *it << ": " << tracker->getParameters().get<string>(*it) << endl;
		}
		// Result processing
		vector<alg::Algorithm*> res_proc_algs = result_processing->getSubFiltersAsAlgs();
		for(vector<alg::Algorithm*>::iterator it = res_proc_algs.begin(); it != res_proc_algs.end(); it++)
		{
			Log::info(0) << "Result processing: " << (*it)->name() << endl;
			vector<string> params = (*it)->getParameters().list();
			for(vector<string>::iterator p_it = params.begin(); p_it != params.end(); p_it++)
				Log::info(0) << "\t" << *p_it << ": " << (*it)->getParameters().get<string>(*p_it) << endl;
		}
		// Detection evaluation
		if(detection_evaluator != NULL)
		{
			Log::info(0) << "Detection evaluation: " << detection_evaluator->name() << endl;
			vector<string> params = detection_evaluator->getParameters().list();
			for(vector<string>::iterator it = params.begin(); it != params.end(); it++)
			{
				Log::info(0) << "\t" << *it << ": " << detection_evaluator->getParameters().get<string>(*it) << endl;
			}
		}
		// Tracking evaluation
		if(tracking_evaluator != NULL)
		{
			Log::info(0) << "Tracking evaluation: " << tracking_evaluator->name() << endl;
			vector<string> params = tracking_evaluator->getParameters().list();
			for(vector<string>::iterator it = params.begin(); it != params.end(); it++)
			{
				Log::info(0) << "\t" << *it << ": " << tracking_evaluator->getParameters().get<string>(*it) << endl;
			}
		}
		Log::info(0) << "Video path: " << video_path << endl;
		Log::info(0) << "Video ID: " << video_id << endl;
		Log::info(0) << "Component ID: " << component_id << endl;
		//Log::info(0) << "Date and time: " << date_time << endl;
		Log::info(0) << "XML output file: " << xml_path << endl;
		Log::info(0) << "Summary frames prefix: " << summary_prefix << endl;
		if(!no_wait)
		{
			// Wait 10 seconds before starting
			Log::debug(0) << "Starting in 10 seconds..." << endl;
			sleep(10);
		}
		// Read number of frames - forget it, OpenCV is not reliable...
		//int num_frames = video_cap.get(CV_CAP_PROP_FRAME_COUNT);
		//cout << "Number of frames: " << num_frames << endl;
		// Configure tracking parameters according to video properties
		//tracker->adaptToVideo(video_cap);
		// Define timers
		TimeInterval frame_timer;
		TimeInterval timer;
		// Define vector for time statistics
		vector<double> frame_times;
		vector<double> detection_times;
		vector<double> tracking_times;
		vector<double> detection_certainty_times;
		vector<double> tracking_certainty_times;
		// Define detection counter
		//int current_detection = 0;
		// Set margin to add to bounding boxes
		int margin = 10;
		// Define detection summary creator - it might not be used though, depending on command-line arguments
		DetectionSummary summary(450, 450);
		summary.setOutputPrefix(summary_prefix);
		// Define list of deleted objects
		list<int> deleted_tracked_objects;
		// Output file for certainties, if required
		ofstream certainty_out_file;
		bool write_certainties = false;
		if(certainty_output.size() > 0)
		{
			write_certainties = true;
			// Open file
			certainty_out_file.open(certainty_output.c_str());
			if(! certainty_out_file.is_open())
			{
				Log::error(0) << "Error opening certainty output file, exiting." << endl;
				updateVideoStatus("error");
				exit(1);
			}
		}
		// Set processing start time
		if(enable_db)
		{
			// Lock tables
			//db_connection->lockTables("processed_videos WRITE");
			processed_video.setStartTime();
			updateVideoStatus("started", false);
			// Unlock tables
			//db_connection->unlockTables();
		}
		// If the detection algorithm is based on XML, set the fps_ratio parameter, so it knows how many frames to skip
		if(detection_alg == "xml")
		{
			detection->getParameters().set("fps_ratio", 1);
		}
		// Load ground truth results, if the XML file has been provided
		Results gt_results;
		if(gt_path.size() > 0)
		{
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
		}
		// Variable for storing number of fish in each frame
		int frame_fish_number;
		// Start processing cycle
		bool first = true;
		for(C.frame_num=0; true; C.frame_num++)
		{
			// Start frame timer
			double frame_time, detection_time, tracking_time, detection_certainty_time, tracking_certainty_time;
			frame_timer.tic();
			try
			{
				// Read frame
				Log::setFrameNumber(C.frame_num);
				if(first)
				{
					first = false;
					video_cap >> C.next_frame;
				}
				C.prev_frame = C.frame.clone();
				C.frame = C.next_frame.clone();
				video_cap >> C.next_frame;
				// Check frame validity
				if(C.frame.rows == 0 || C.frame.cols == 0 || C.frame.data == NULL)
				{
					Log::warning() << ShellUtils::RED << "Invalid frame: " << C.frame_num << ", closing video..." << ShellUtils::NORMAL << endl;
					break;
				}
				// Skip frames
				if(C.frame_num < process_from)
				{
					continue;
				}
				if(process_to > 0 && C.frame_num > process_to)
				{
					break;
				}
				Log::debug() << "Processing frame " << C.frame_num << endl;
				// Check video progress update
				if(total_frames > 0 && (C.frame_num % 750) == 0)
				{
					float progress = ((float)C.frame_num)/((float)total_frames)*100.f;
					Log::debug() << "Computing progress: " << progress << " (current frame: " << C.frame_num << ", total frames: " << total_frames << ")" << endl;
					//cout << "before updatevideoprogress" << endl; sleep(10);
					updateVideoProgress((C.frame_num/((float)total_frames))*100.0f, true);
					//cout << "after updatevideoprogress" << endl; sleep(10);
				}
				// Blacken time
				C.frame_no_time = C.frame.clone();
				Rect rect(0, 0, 160, 15);
				for(int i=rect.y; i<(rect.y + rect.height); i++)
				{
					for(int j=rect.x; j<(rect.x + rect.width); j++)
					{
						C.frame_no_time.at<Vec3b>(i,j) = Vec3b(0, 0, 0);
					}			
				}
				// Apply pre-processing
				C.preproc_output = pre_processing->nextFrame(C.frame_no_time);
				// Copy to drawing frame
				if(enable_debug && C.frame_num >= debug_from && C.frame_num <= debug_to)
				{
					C.frame.copyTo(draw_frame);
				}
				// Convert frame to IplImage
				//IplImage ipl_frame = C.frame;
				// Run detection algorithm
				timer.tic();
				detection->nextFrame(C);
				detection_time = timer.toc();
				detection_times.push_back(detection_time);
				// Apply post-processing
				post_processing->nextFrame(C);
				// Compute blobs
				C.original_blobs = computeBlobs(C.postproc_output);
				// Remove invalid blobs
				BlobProcessing::removeInvalidBlobs(C.original_blobs, C.frame.cols, C.frame.rows);
				// Apply blob processing
				blob_processing->filterBlobs(C.frame_no_time, C.original_blobs, C.filtered_blobs, &C.next_frame);
				/*// Show all blobs
				for(cvb::CvBlobs::const_iterator b_it = C.filtered_blobs.begin(); b_it != filtered_blobs.end(); b_it++)
				{
					showBlob("filtered", *b_it->second);
					waitKey(0);
				}*/
				frame_fish_number = C.filtered_blobs.size();
				Log::debug() << "Found " << frame_fish_number << " fish." << endl;
				// Check if we have to save the video preview
				if(save_video_preview && frame_fish_number == 0 && C.frame_num > 500)
				{
					try
					{
						imwrite("preview.png", C.frame);
						save_video_preview = false;
					}
					catch(...)
					{
						Log::error() << ShellUtils::RED << "Error saving video preview." << ShellUtils::NORMAL << endl;
					}
				}
				// Save blobs to summary image
				if(enable_summary_images)
				{	
					Mat blob_image = rebuildBlobs(C.filtered_blobs, C.frame.cols, C.frame.rows);
					for(cvb::CvBlobs::const_iterator it=C.filtered_blobs.begin(); it!=C.filtered_blobs.end(); it++)
					{
						// Get blob and bounding box
						cvb::CvBlob& orig_blob = *(it->second);
						cvb::CvBlob blob;
						cvCloneBlob(orig_blob, blob);
						addMargin(blob, margin, C.frame.rows, C.frame.cols);
						Rect box(blob.x, blob.y, blob.width(), blob.height());
						// Add detection
						summary.addDetection(C.frame(box), blob_image(box));
					}
				}
				// Run tracking algorithm
				timer.tic();
				tracker->nextFrame(C.frame_no_time, C.frame_num, C.filtered_blobs);
				tracking_time = timer.toc();
				int num_currently_tracked_objects = tracker->getTrackedObjects().size();
				if(num_currently_tracked_objects > 0)
				{
					for(int i=0; i<(int)(tracker->getTrackedObjects().size()); ++i)
					{
						tracking_times.push_back(tracking_time/tracker->getTrackedObjects().size());
					}
				}
				// Get current tracked objects
				const vector<const TrackedObject*> tracked_objects = tracker->getTrackedObjects();
				Log::info() << "Tracked objects: ["; for(unsigned int i=0; i<tracked_objects.size(); i++) Log::info(0) << tracked_objects[i]->id << " "; Log::info(0) << "]" << endl;
				Log::info() << "Lost objects: [" << tracker->getLostObjects().size() << " objects ]" << endl;
				// Perform evaluation
				if(detection_evaluator != NULL)
				{
					timer.tic();
					//((BayesDetectionEvaluator*)detection_evaluator)->addFrame(tracked_objects, C.frame, C.frame_num, &C.next_frame);
					detection_evaluator->addFrame(tracked_objects, C.frame, C.frame_num, &C.next_frame);
					detection_certainty_time = timer.toc();
					if(frame_fish_number > 0)
					{	
						for(int i=0; i<frame_fish_number; ++i)
						{
							detection_certainty_times.push_back(detection_certainty_time/frame_fish_number);
						}
					}
				}
				if(tracking_evaluator != NULL)
				{
					timer.tic();
					tracking_evaluator->addFrame(tracked_objects, C.frame, C.frame_num, &C.next_frame);
					tracking_certainty_time = timer.toc();
					if(frame_fish_number > 0)
					{
						for(int i=0; i<frame_fish_number; ++i)
						{
							tracking_certainty_times.push_back(tracking_certainty_time/frame_fish_number);
						}
					}
				}
				// Add detections to results
				for(unsigned int i=0; i<tracked_objects.size(); i++)
				{
					// Get tracked object
					const TrackedObject* tracked_object = tracked_objects[i];
					if(tracked_object->lastAppearance() == C.frame_num)
					{
						// Add to results (we add objects even if they have less the minimum_appearances appearances, we'll check it later on)
						pr::Object object;
						object.setId(tracked_object->id);
						object.setFrameNumber(C.frame_num); // Use actual frame number
						vector<Point> bounding_box, contour;
						blobToBoundingBoxAndContour(tracked_object->currentRegion(), bounding_box, contour);
						//object.setBoundingBox(bounding_box);
						object.setContour(contour);
						if(detection_evaluator != NULL)
						{
							Score* detection_certainty = detection_evaluator->getScore(object.getId(), object.getFrameNumber());
							if(detection_certainty != NULL)
							{
								object.setDetectionCertainty(detection_certainty->value());
							}
						}
						if(tracking_evaluator != NULL)
						{
							Score* tracking_certainty = tracking_evaluator->getScore(object.getId(), object.getFrameNumber());
							if(tracking_certainty != NULL)
							{
								object.setTrackingCertainty(tracking_certainty->value());
							}
						}
						C.results.addObject(object);
					}
				}

				// Draw objects on a copy of the frame
				if(enable_debug && C.frame_num >= debug_from && C.frame_num <= debug_to)
				{	
					// Draw ground truth results			
					ResultsUtils::drawObjects(gt_results, draw_frame, C.frame_num, CV_RGB(0,255,0), 0, 2);
					cur_tracked_objects.clear();
					for(unsigned int i=0; i<tracked_objects.size(); i++)
					{
						// Get object
						const TrackedObject& tracked_object = *(tracked_objects[i]);
						// Draw stuff about this object
						tracked_object.drawContour(draw_frame, C.frame_num, CV_RGB(255,0,0));				
						if (tracked_object.lastAppearance() == C.frame_num)
							cur_tracked_objects.push_back(tracked_objects[i]);
						// Get blob
						//const cvb::CvBlob& blob = tracked_object.currentRegion();
						// Draw rectangle
						//rectangle(draw_frame, Point(blob.minx, blob.miny), Point(blob.maxx, blob.maxy), Scalar(0,0,255));
					}
					// Draw tracks
					//tracker->drawTracks(draw_frame); 
	    			
					if(tracker->numTrackedObjects() > 0 || gt_results.getObjectsByFrame(C.frame_num).size() > 0 || debug_level > 1)
					{
						writer.write(draw_frame);
						string win_name = boost::to_string(C.frame_num);
						imshow(win_name+"frame_win", draw_frame);
						cvSetMouseCallback("frame_win", on_mouse, 0); // ADD this line	
						// Check other things to show
						if(show_motion_output)
						{
							imshow("motion", C.motion_output);
						}
						if(show_postproc_output)
						{
							imshow("postproc", C.postproc_output);
						}
						waitKey(0);
					}
					else
					{
						waitKey(1);
					}
				}
				// Cleanup memory
				cvb::cvReleaseBlobs(C.original_blobs);
				cvb::cvReleaseBlobs(C.filtered_blobs);
			}
			catch(MyException& e)
			{
				frames_with_errors++;
				Log::error() << ShellUtils::RED << "Error in main at frame " << C.frame_num << ": " << e.what() << ShellUtils::NORMAL << endl;
				if(enable_debug && C.frame_num >= debug_from && C.frame_num <= debug_to)
				{
					waitKey(1);
				}
			}
			catch(exception& e)
			{
				frames_with_errors++;
				Log::error() << ShellUtils::RED << "Error in main at frame " << C.frame_num << ": " << e.what() << ShellUtils::NORMAL << endl;
				if(enable_debug && C.frame_num >= debug_from && C.frame_num <= debug_to)
				{
					waitKey(0);
				}
			}
			catch(...)
			{
				frames_with_errors++;
				Log::error() << ShellUtils::RED << "Error in main at frame " << C.frame_num << "." << ShellUtils::NORMAL << endl;
				if(enable_debug && C.frame_num >= debug_from && C.frame_num <= debug_to)
				{
					waitKey(0);
				}
			}
			// Stop frame time
			frame_time = frame_timer.toc();
			frame_times.push_back(frame_time);
			// Print time statistics
			Log::info() << "Frame processing time: " << frame_time << " (detection: " << detection_time << "; tracking: " << tracking_time;
			if(detection_evaluator != NULL)
			{
				Log::info(0) << "; detection certainty: " << detection_certainty_time;
			}
			if(tracking_evaluator != NULL)
			{
				Log::info(0) << "; tracking certainty: " << tracking_certainty_time;
			}
			Log::info(0) << ") s, for " << frame_fish_number << " objects." << endl;
			//cout << "before frame end" << endl; sleep(10);
		}
		//cout << "before setting processing end" << endl; sleep(10);
		// Set processing end info to db
		if(enable_db)
		{
			// Lock tables
			//db_connection->lockTables("processed_videos WRITE");
			processed_video.setEndTime();
			// Set progress to 100
			updateVideoProgress(100, false);
			//db_connection->unlockTables();
		}
		//cout << "after setting processing end" << endl; sleep(10);
		Log::info(0) << "Video processing completed, saving results to database and XML." << endl;
		//cout << "before saving_data" << endl; sleep(10);
		updateVideoStatus("saving_data");
		//cout << "after saving_data" << endl; sleep(10);
		// Apply result processing
		result_processing->processResults(C);
		// Write to certainty file
		list<int> object_ids = C.filtered_results.idList();
		for(list<int>::iterator it = object_ids.begin(); it != object_ids.end(); it++)
		{
			// Get object group
			ObjectList objects = C.filtered_results.getObjectsById(*it);
			// Go through all detections
			for(ObjectList::iterator it2 = objects.begin(); it2 != objects.end(); it2++)
			{
				// Write certainty scores to file, if required
				if((detection_evaluator != NULL || tracking_evaluator != NULL) && write_certainties && it2 != objects.begin())
				{
					certainty_out_file << it2->getDetectionCertainty() << "\t" << it2->getTrackingCertainty() << endl;
				}
			}
		}
		// Write to database
		bool write_to_db_completed = false;
		try
		{
			if(enable_db && !no_db_results)
			{
				// Initialize group insertion vector
				vector<db::DBEntity*> fish_detection_group_insert;
				// Get object ids
				object_ids = C.filtered_results.idList();
				Log::d() << "Adding detections to group insert list." << endl;
				for(list<int>::iterator it = object_ids.begin(); it != object_ids.end(); it++)
				{
					// Get object id
					int object_id = *it;
					// Create fish entity
					db::Fish fish(db_connection);
					fish.setField<string>("best_video_id", video_id);
					fish.setField<int>("component_id", component_id);
					bool fish_complete = false; // we still have to add best_contour, best_bounding_box and best_frame
					// Get detections
					ObjectList detections = C.filtered_results.getObjectsById(object_id);
					for(ObjectList::iterator o_it = detections.begin(); o_it != detections.end(); o_it++)
					{
						// Get detection
						Object& detection = *o_it;
						// Complete fish with this detection, if needed
						if(!fish_complete)
						{
							// Fill missing fields
							fish.setField<int>("best_frame", detection.getFrameNumber());
							//fish.setField<vector<Point> >("best_bounding_box", detection.getBoundingBox());
							//fish.setField<vector<Point> >("best_contour", detection.getContour());
							fish.setField<string>("best_bb_cc", db::ContourEncoder::encode(detection.getContour()));
							// Insert into database
							fish.insertToDB();
							// Mark as complete
							fish_complete = true;
						}
						// Create fish detection entity
						db::FishDetection* fish_detection = new db::FishDetection(db_connection);
						fish_detection->setField<int>("fish_id", fish.getField<int>("fish_id"));
						fish_detection->setField<string>("video_id", video_id);
						fish_detection->setField<int>("frame_id", detection.getFrameNumber());
						//fish_detection.setField<vector<Point> >("bounding_box", detection.getBoundingBox());
						//fish_detection.setField<vector<Point> >("contour", detection.getContour());
						fish_detection->setField<string>("bb_cc", db::ContourEncoder::encode(detection.getContour()));
						fish_detection->setField<int>("component_id", component_id);
						fish_detection->setField<float>("detection_certainty", detection.getDetectionCertainty());
						fish_detection->setField<float>("tracking_certainty", detection.getTrackingCertainty());
						// Insert into group insertion structure
						fish_detection_group_insert.push_back(fish_detection);
					}
				}
				Log::d() << "Calling group insert." << endl;
				// Insert all fish detections
				db::DBEntity::groupInsert(fish_detection_group_insert, db_connection);
				// Set success
				write_to_db_completed = true;
			}
		}
		catch(MyException& e)
		{
			Log::error(0) << "Error while saving results to db: " << e.what() << endl;
			updateVideoStatus("error");
		}
		catch(exception& e)
		{
			Log::error(0) << "Error while saving results to db: " << e.what() << endl;
			updateVideoStatus("error");
		}
		catch(...)
		{
			Log::error(0) << "Error while saving results to db." << endl;
			updateVideoStatus("error");
		}
		// Write XML results
		XMLTDResults xml_results;
		xml_results.writeFile(xml_path, C.filtered_results, VideoInfo());
		Log::debug(0) << "XML file written." << endl;
		// Set final video status
		if(write_to_db_completed || no_db_results)
		{
			updateVideoStatus("completed");
		}
		// Compute time statistics
		float avg_frame_time, std_frame_time;
		float avg_detection_time, std_detection_time;
		float avg_tracking_time, std_tracking_time;
		float avg_detection_certainty_time, std_detection_certainty_time;
		float avg_tracking_certainty_time, std_tracking_certainty_time;
		MathUtils::computeMeanAndStdDev<double>(frame_times, avg_frame_time, std_frame_time);
		MathUtils::computeMeanAndStdDev<double>(detection_times, avg_detection_time, std_detection_time);
		MathUtils::computeMeanAndStdDev<double>(tracking_times, avg_tracking_time, std_tracking_time);
		MathUtils::computeMeanAndStdDev<double>(detection_certainty_times, avg_detection_certainty_time, std_detection_certainty_time);
		MathUtils::computeMeanAndStdDev<double>(tracking_certainty_times, avg_tracking_certainty_time, std_tracking_certainty_time);
		Log::info(0) << "Average frame time: " << avg_frame_time << " +- " << std_frame_time << endl;
		Log::info(0) << "Average detection time: " << avg_detection_time << " +- " << std_detection_time << endl;
		Log::info(0) << "Average tracking time (per object): " << avg_tracking_time << " +- " << std_tracking_time << endl;
		Log::info(0) << "Average detection certainty time (per object): " << avg_detection_certainty_time << " +- " << std_detection_certainty_time << endl;
		Log::info(0) << "Average tracking certainty time (per object): " << avg_tracking_certainty_time << " +- " << std_tracking_certainty_time << endl;
		// Close certainty output file, if opened
		if(write_certainties)
		{
			certainty_out_file.close();
		}
		// Free stuff
		if(enable_db)
		{
			delete db_connection;
		}
		vector<PreProcessing*> pre_proc_filters = pre_processing->getSubFilters();
		for(vector<PreProcessing*>::iterator it = pre_proc_filters.begin(); it != pre_proc_filters.end(); it++)
		{
			delete *it;
		}
		delete pre_processing;
		vector<BlobProcessing*> blob_proc_filters = blob_processing->getSubFilters();
		for(vector<BlobProcessing*>::iterator it = blob_proc_filters.begin(); it != blob_proc_filters.end(); it++)
		{
			delete *it;
		}
		delete blob_processing;
		delete detection;
		delete tracker;
		if(detection_evaluator != NULL)
		{
			delete detection_evaluator;
		}
		if(tracking_evaluator != NULL)
		{
			delete tracking_evaluator;
		}
		delete software_component;
		// Close capture
		// ???
		// Done!
		Log::info(0) << "Done!" << endl;
	}
	catch(MyException& e)
	{
		updateVideoStatus("error");
		Log::error() << ShellUtils::RED << "Error: " << e.what() << ShellUtils::NORMAL << endl;
	}
	catch(exception& e)
	{
		updateVideoStatus("error");
		Log::error() << ShellUtils::RED << "Error: " << e.what() << ShellUtils::NORMAL << endl;
	}
	catch(...)
	{
		updateVideoStatus("error");
		Log::error() << ShellUtils::RED << "Error." << ShellUtils::NORMAL << endl;
	}
}

void addMargin(cvb::CvBlob& blob, unsigned int margin, unsigned int height, unsigned int width)
{
	if(blob.x < margin) blob.x = 0; else blob.x = blob.x - margin;
	if(blob.y < margin) blob.y = 0; else blob.y = blob.y - margin;
	blob.maxx = blob.maxx + margin; if(blob.maxx >= width) blob.maxx = width-1;
	blob.maxy = blob.maxy + margin; if(blob.maxy >= height) blob.maxy = height-1;
}
