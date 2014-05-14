// TODO iterators
#ifndef KEYFRAMES_H
#define KEYFRAMES_H

#include <list>
#include <algorithm>
#include <iostream>
#include <climits>
#include <log.h>

// Notes:
// - NOT thread-safe
// - the algorithm tries some attempts to see if it can quickly compute the result (i.e. check against max keyframe value and against most recently matched interval); otherwise, it goes through all ORDERED intervals.
// - if no intervals are added, return true by default

struct KeyFrameInterval
{
	// Label
	std::string label;
	// Interval
	std::pair<int,int> interval;

	// Constructors
	inline KeyFrameInterval() : label(""), interval(-1, -1) {}
	inline KeyFrameInterval(const std::pair<int,int>& new_interval) : label(""), interval(new_interval) {}
	inline KeyFrameInterval(const std::pair<int,int>& new_interval, const std::string& new_label) : label(new_label), interval(new_interval) {}
	inline KeyFrameInterval(int start, int end) : label(""), interval(start, end) {}
	inline KeyFrameInterval(int start, int end, const std::string& new_label) : label(new_label), interval(start, end) {}
	inline KeyFrameInterval(const KeyFrameInterval& copy) : label(copy.label), interval(copy.interval) {}

	// Get interval
	inline int start() const { return interval.first; }
	inline int end() const { return interval.second; }

	// Assignment operator
	KeyFrameInterval& operator=(const KeyFrameInterval& copy)
	{
		if(this == &copy) return *this;
		label = copy.label;
		interval = copy.interval;
		return *this;
	}
};

class KeyFrames
{
	private:

	// Intervals
	std::list<KeyFrameInterval> intervals;
	// Max value
	int max_value;
	// Current interval
	KeyFrameInterval current;

	// Comparison class
	class Comparer
	{
		public:
			
			inline bool operator()(KeyFrameInterval p1, KeyFrameInterval p2)
			{
				if(p1.start() < p2.start())	return true;
				if(p2.start() < p1.start())	return false;
				if(p1.end() < p2.end())		return true;
				if(p2.end() < p1.end())		return false;
				return true;
			}
	};

	protected:

	public:

	friend std::ostream& operator<<(std::ostream& out, const KeyFrames& kf);

	// Constructor
	KeyFrames() :
		max_value(INT_MIN), current(-1,-1)
	{
	}

	// Add interval
	inline void add(int start, int end) { add(start, end, ""); }
	void add(int start, int end, std::string label);

	// Check frame
	bool check(int frame, std::string label = "");
	bool check(int frame, std::string label = "") const; // This version does not update the "current" interval

	// Get intervals
	inline std::list<KeyFrameInterval> getIntervals() const { return intervals; }
	
	// Get labels
	std::list<std::string> getFrameLabels(int frame) const;

	// Get labels
	std::list<std::string> listLabels() const;

	// Get list of frame
	std::list<int> listFrames() const;
};

#endif
