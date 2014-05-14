// Class which represents the score history for an object.

#include "score_history.h"
using namespace std;

namespace alg
{

	// Destructor; frees scores
	ScoreHistory::~ScoreHistory()
	{
		// Get single scores
		for(map<int, Score*>::iterator it = scores.begin(); it != scores.end(); it++)
		{
			// Free variable
			delete it->second;
		}

	}

	// Get an object score for a specified frame
	// Return -1 in case of errors
	// - frame: frame number
	Score* ScoreHistory::getScore(int frame)
	{
		// Look for frame
		map<int, Score*>::iterator it = scores.find(frame);
		// Check if it exists
		if(it == scores.end())
		{
			return NULL;
		}
		// Return score
		return it->second;
	}

	// Compute the overall score for the object
	Score ScoreHistory::getOverallScore()
	{
		// Initialize result
		float score = 0.0f;
		// Get score from each frame
		for(map<int, Score*>::iterator it = scores.begin(); it != scores.end(); it++)
		{
			// Add score to sum
			score += it->second->value();
		}
		// Divide by number of frames
		score /= scores.size();
		// Return as a Score object
		return Score(score);
	}

}
