// Class which represents the score history for an object.

#ifndef SCORE_HISTORY_H
#define SCORE_HISTORY_H

#include <map>
#include "score.h"
#include <log.h>

namespace alg
{

	class ScoreHistory
	{
		protected:

		// Map between frame numbers and score values
		std::map<int, Score*> scores;

		public:

		// Destructor; frees scores
		virtual ~ScoreHistory();

		// Add a new score for the specified frame
		// - score: score value
		// - frame_num: frame number
		inline void addScore(Score* score, int frame_number) { scores[frame_number] = score; }

		// Number of scores in history
		inline int numScores() { return scores.size(); }

		// Get an object score for a specified frame
		// Return NULL in case of errors
		// - frame: frame number
		Score* getScore(int frame);

		// Compute the overall score for the object
		Score getOverallScore();
	};

}

#endif
