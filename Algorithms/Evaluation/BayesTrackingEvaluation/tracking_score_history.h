#ifndef TRACKING_SCORE_HISTORY_H
#define TRACKING_SCORE_HISTORY_H

#include <score_history.h>

namespace alg
{

	class TrackingScoreHistory : public ScoreHistory
	{
		private:

		// Accumulated velocity, used to compute an average
		float accumulated_velocity;

		public:

		// Constructor
		TrackingScoreHistory() : ScoreHistory(), accumulated_velocity(0.0f)
		{
		}

		// Add velocity
		inline void accumulateVelocity(float velocity) { accumulated_velocity += velocity; }

		// Get average velocity
		inline float getAverageVelocity() { return accumulated_velocity/(scores.size()-1); }
	};

}

#endif
