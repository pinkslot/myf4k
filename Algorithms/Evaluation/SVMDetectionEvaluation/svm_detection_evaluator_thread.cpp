#include "svm_detection_evaluator.h"
#include "svm_detection_score.h"
#include <cvblob.h>
#include <lk_motion_vector.h>
#include <motion_vector_utils.h>
#include <time_utils.h>
#include <log.h>

using namespace alg;
using namespace cv;
	
void SVMDetectionEvaluator::computeScoreThread(const cvb::CvBlob& blob, const Mat& frame, SVMDetectionScore& score, const Mat* next_frame)
{
	bool blob_deleted = false;
	try
	{
		// Compute boundary complexity score
		computeBoundaryComplexityScore(blob, frame.rows, frame.cols, score.bc);
		// Compute color contrast score
		computeColorContrastScore(frame, blob, score.cc);
		// Compute superpixel straddling score
		computeSuperpixelsStraddlingScore(frame, blob, score.ss);
		// Compute motion boundary score
		computeMotionBoundaryScore(frame, next_frame, blob, score.mb);
		// Compute motion internal score
		//computeMotionInternalScore(frame, next_frame, blob, score.mi);
		// Compute color internal score
		//computeColorInternalScore(frame, blob, score.ci);
		// Print score
		log_mutex.lock();
		Log::d() << "Detection score: " << score.value() << endl;
		Log::d() << "\tbc: " << score.bc << endl;
		Log::d() << "\tcc: " << score.cc << endl;
		Log::d() << "\tss: " << score.ss << endl;
		Log::d() << "\tmb: " << score.mb << endl;
		//Log::d() << "\tmi: " << score.mi << endl;
		//Log::d() << "\tci: " << score.ci << endl;
		log_mutex.unlock();
		// Delete blob (it was allocate by addFrame())
		delete &blob;
		blob_deleted = true;
	}
	catch(boost::thread_resource_error& e)
	{
		Log::e() << ShellUtils::RED << "Error in certainty computation: maybe thread related: " << e.what() << endl;
		// Free resources
		if(!blob_deleted)			delete &blob;
	}
	catch(exception& e)
	{
		Log::e() << ShellUtils::RED << "Error in certainty computation: maybe thread related: " << e.what() << endl;
		// Free resources
		if(!blob_deleted)			delete &blob;
	}
	catch(...)
	{
		Log::e() << ShellUtils::RED << "Error in certainty computation: maybe thread related." << endl;
		// Free resources
		if(!blob_deleted)			delete &blob;
	}
}
