// Class which represents a generic tracking evaluator
// For each tracked object, it stores the sequence of evaluation scores for each frame in which it was recognized and also computes a global score for the whole history

#include "evaluator.h"

namespace alg
{

	// Constructor:
	// - frame_buffer_size: number of previous frames to be stored, in order to compare them with objects recognized in the current frame but which have been missing for a while
	Evaluator::Evaluator(int frame_buffer_size)
	{
		// Allocate space
		frame_buffer = new CircularBuffer<Mat>(frame_buffer_size);
	}
		
	// Destructor; frees buffer and score histories
	Evaluator::~Evaluator()
	{
		// Free frame buffer
		delete frame_buffer;
		// Free score histories
		for(map<int, ScoreHistory*>::iterator it = objects.begin(); it != objects.end(); it++)
		{
			delete it->second;
		}
	}

	// Get an object score history
	const ScoreHistory* Evaluator::getScoreHistory(int object_id)
	{
		// Look for specified ID
		map<int, ScoreHistory*>::iterator it = objects.find(object_id);
		// Check if it exists
		if(it == objects.end())
		{
			return NULL;
		}
		// Return object history
		return it->second;
	}

	// Get an object score for a specified frame
	// Return -1 in case of errors
	// - object_id: object ID
	// - frame: frame number
	Score* Evaluator::getScore(int object_id, int frame)
	{
		// Look for the specified ID
		map<int, ScoreHistory*>::iterator it = objects.find(object_id);
		// Check if it exists
		if(it == objects.end())
		{
			return NULL;
		}
		// Return score (TrackedObjectScore deals with the validity of frame, returning -1 in case)
		return it->second->getScore(frame);
	}

	// Compute the overall score for the whole execution
	Score Evaluator::getOverallScore()
	{
		// Initialize result
		float score = 0.0f;
		// Read each object's overall score
		for(map<int, ScoreHistory*>::iterator it = objects.begin(); it != objects.end(); it++)
		{
			// Add it to the sum
			score += it->second->getOverallScore().value();
		}
		// Divide by the number of items
		score /= objects.size();
		// Return as a Score object
		return Score(score);
	}

}
