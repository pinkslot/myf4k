#include "keyframes.h"

using namespace std;

// Add interval
void KeyFrames::add(int start, int end, string label)
{
	// Check order
	if(start > end)
	{
		Log::w() << "KeyFrames: start > end, reversing." << endl;
		int tmp = end;
		end = start;
		start = tmp;
	}
	// Add to intervals
	intervals.push_back(KeyFrameInterval(start, end, label));
	// Check max
	if(end > max_value)	max_value = end;
	// Sort list
	Comparer comp;
	intervals.sort(comp);
}

// Check frame
bool KeyFrames::check(int frame, string label)
{
	// Check intervals size
	if(intervals.size() == 0)
	{
		return true;
	}
	// Check max
	if(frame > max_value)
	{
		return false;
	}
	// Check current
	if((label == "" || label == current.label) && frame >= current.start() && frame <= current.end())
	{
		return true;
	}
	// Check all intervals
	for(list<KeyFrameInterval>::const_iterator it = intervals.begin(); it != intervals.end(); it++)
	{
		// Check label
		if(label != "" && label != it->label)
		{
			continue;
		}
		// Check min value
		if(frame < it->start())
		{
			return false;
		}
		// Check interval
		if(frame >= it->start() && frame <= it->end())
		{
			// Save current interval
			current = *it;
			// Return true
			return true;
		}
	}
	// Not found
	return false;
}

// Check frame
bool KeyFrames::check(int frame, string label) const
{
	// Check intervals size
	if(intervals.size() == 0)
	{
		return true;
	}
	// Check max
	if(frame > max_value)
	{
		return false;
	}
	// Check current
	if((label == "" || label == current.label) && frame >= current.start() && frame <= current.end())
	{
		return true;
	}
	// Check all intervals
	for(list<KeyFrameInterval>::const_iterator it = intervals.begin(); it != intervals.end(); it++)
	{
		// Check label
		if(label != "" && label != it->label)
		{
			continue;
		}
		// Check min value
		if(frame < it->start())
		{
			return false;
		}
		// Check interval
		if(frame >= it->start() && frame <= it->end())
		{
			// DON'T SAVE CURRENT INTERVAL!
			// Return true
			return true;
		}
	}
	// Not found
	return false;
}
	
// Get labels
list<string> KeyFrames::listLabels() const
{
	// Initialize output list
	list<string> out;
	// Go through all intervals
	for(list<KeyFrameInterval>::const_iterator it = intervals.begin(); it != intervals.end(); it++)
	{
		// Get label
		string label = it->label;
		// Check if label already exists or if it is empty
		if(label != "" && find(out.begin(), out.end(), label) == out.end())
		{
			// Add label
			out.push_back(label);
		}
	}
	// Return result
	return out;
}
	
// Get list of frame
list<int> KeyFrames::listFrames() const
{
	// Initialize output list
	list<int> out;
	// Go through all intervals
	for(list<KeyFrameInterval>::const_iterator it = intervals.begin(); it != intervals.end(); it++)
	{
		// Get start and end
		int start = it->start();
		int end = it->end();
		// Go through all numbers between
		for(int i=start; i<=end; i++)
		{
			// Check if the frame already exists
			if(find(out.begin(), out.end(), i) == out.end())
			{
				// Add frame
				out.push_back(i);
			}
		}
	}
	// Sort list
	out.sort();
	// Return result
	return out;
}
	
// Get labels
list<string> KeyFrames::getFrameLabels(int frame) const
{
	// Get labels
	list<string> labels = listLabels();
	// Initialize output list
	list<string> out;
	// Check each label
	for(list<string>::iterator l_it = labels.begin(); l_it != labels.end(); l_it++)
	{
		if(check(frame, *l_it))
		{
			out.push_back(*l_it);
		}
	}
	// Return result
	return out;
}
	
ostream& operator<<(ostream& out, const KeyFrames& kf)
{
	for(list<KeyFrameInterval>::const_iterator it = kf.intervals.begin(); it != kf.intervals.end(); it++)
	{
		if(it->start() != it->end())
			out << it->start() << "-" << it->end() << " (" << it->label << ") ";
		else
			out << it->start() << " (" << it->label << ") ";
	}
	return out;
}
