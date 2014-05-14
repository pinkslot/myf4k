#include "svm_detection_evaluator.h"
#include <lk_motion_vector.h>
#include <opencv_utils.h>
#include <svm_utils.h>
#include <iostream>
#include <log.h>

using namespace alg;
using namespace std;

int main()
{
	try
	{
		Log::setDebugLogStream(cout);
		Log::setInfoLogStream(cout);
		Log::setWarningLogStream(cout);
		Log::setErrorLogStream(cerr);
		SVMDetectionEvaluator svm_eval;
		Mat frame = imread("frame_53.png");
		Mat mask_bgr = imread("mask_53.png");
		Mat mask;
		cvtColor(mask_bgr, mask, CV_BGR2GRAY);
		Mat next = imread("next_53.png");
		cvb::CvBlobs blobs = computeBlobs(mask);
		cvb::CvBlob& blob = *(blobs.begin()->second);
		float result;
		/*LKMotionVector mv;
		mv.window_size = 20;
		mv.max_level = 0;
		Mat motion_vector;
		Rect curr_rect;
		curr_rect = Rect(blob.x, blob.y, blob.width(), blob.height());
		int incr = 0;//15;
		curr_rect -= Point(incr, incr);
		curr_rect += Size(2*incr, 2*incr);
		if(curr_rect.x < 0)					curr_rect.x = 0;
		if(curr_rect.y < 0)					curr_rect.y = 0;
		if(curr_rect.x + curr_rect.width - 1 >= frame.cols)	curr_rect.width = frame.cols - curr_rect.x;
		if(curr_rect.y + curr_rect.height - 1 >= frame.rows)	curr_rect.height = frame.rows - curr_rect.y;
		MotionVector* mv2 = &mv;
		// Compute motion vector
		mv2->addFrame(frame);
		mv2->addFrame(next, curr_rect, motion_vector);
		svm_eval.computeMotionInternalScore(motion_vector, blob, result);*/
		//svm_eval.computeMotionInternalScore(frame, &next, blob, result);
		//cout << result << endl;
		SVMUtils svm;
		svm.loadBuiltIn();
		SVMDetectionScore score(&svm);
		svm_eval.computeScoreThread(blob, frame, score, &next);
		cout << score.value() << endl;
	}
	catch(MyException& e)
	{
		cout << e.what() << endl;
	}
}
