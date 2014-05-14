// Class which represents a generic evaluator
// For each tracked object, it stores the sequence of evaluation scores for each frame in which it was recognized and also computes a global score for the whole history

#ifndef ALG_EVALUATOR_H
#define ALG_EVALUATOR_H

#include <map>
#include <cv.h>
#include <tracked_object.h>
#include <opencv_utils.h>
#include "score_history.h"
#include "../algorithm.h"

namespace alg
{

	class Evaluator : public Algorithm
	{
		protected:

		// Map between tracked object IDs and TrackedObjectScore objects
		map<int, ScoreHistory*> objects;

		// Frame buffer
		CircularBuffer<Mat> *frame_buffer;

		public:

		virtual string name() const = 0;				// e.g. gmm, apmm
		virtual string type() const = 0;				// e.g. detection, tracking, recognition
		virtual string description() const = 0;				// e.g. algorithm description
		virtual string version() const = 0;				// e.g. 1.2
		virtual int executionTime() const = 0;				// time (in milliseconds) to process an input unit (e.g. frame for detection algorithms, object for tracking algorithms)
		virtual string descriptionExecutionTime() const = 0;		// description of how the execution time is computer
		virtual int ram() const = 0;					// average amount of RAM used by the algorithm in MB
		inline string input() const { return "vector<TrackedObject>"; }
		inline string output() const { return "ScoreHistory"; }
		
		// Constructor:
		// - frame_buffer_size: number of previous frames to be stored, in order to compare them with objects recognized in the current frame but which have been missing for a while
		Evaluator(int frame_buffer_size = 5);

		// Given the list of currently tracked object, perform tracking evaluation on the current frame
		// - tracked_objects: list of tracked objects
		// - frame: frame matrix
		// - frame_num: frame number
		virtual void addFrame(const vector<const alg::TrackedObject*>& tracked_objects, const cv::Mat& frame, int frame_number, const cv::Mat* next_frame) = 0;

		// Create a new tracked object score and add it to the list:
		// - object_id: tracked object (as managed by the tracker) ID
		//void addObject(int object_id);

		// Set the score for an object on a specific frame:
		// - object_id
		// - frame: number of frame to which this score is related
		// - score: evaluated score
		//void setScore(int object_id, int frame, float score);

		// Get an object score history
		const ScoreHistory* getScoreHistory(int object_id);

		// Get an object score for a specified frame
		// Return NULL in case of errors
		Score* getScore(int object_id, int frame);

		// Compute the overall score for the whole execution
		Score getOverallScore();

		// Destructor; frees buffer and score histories
		virtual ~Evaluator();
	};

}

#endif
