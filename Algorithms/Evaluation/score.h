// Represents an evaluation score; it basically provides a value() method.

#ifndef SCORE_H
#define SCORE_H

namespace alg
{

	class Score
	{
		protected:

		// Provides a simple way to set/get a score; is the value returned by the default implementation of value().
		float simple_value;

		public:

		// Constructor; takes as input the score
		inline Score(float init_value = 0) : simple_value(init_value) {}

		// Return the actual score; by default, returns simple_value.
		virtual inline float value() { return simple_value; }
	};

}

#endif
