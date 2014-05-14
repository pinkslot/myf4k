// Output results of detection/tracking
// TODO improve memory usage
// TODO iterators

#include "results.h"
#include "my_exception.h"
#include "string_utils.h"

using namespace std;

namespace pr
{
	// Get all objects in a frame
	ObjectList Results::getObjectsByFrame(int frame_number) const
	{
		ObjectGroups::const_iterator it = frame_map.find(frame_number);
		if(it == frame_map.end())
			return ObjectList();
		return it->second;
	}

	// Get all objects by id
	ObjectList Results::getObjectsById(int id) const
	{
		ObjectGroups::const_iterator it = id_map.find(id);
		if(it == id_map.end())
			return ObjectList();
		return it->second;
	}
		
	// Get object by id and frame number
	Object Results::getObject(int id, int frame) const
	{
		// Get objects by id
		ObjectList objects = getObjectsById(id);
		// Look up frame
		for(ObjectList::iterator it = objects.begin(); it != objects.end(); it++)
		{
			if(it->getFrameNumber() == frame)
			{
				return *it;
			}
		}
		// Nothing found, throw exception
		throw MyException("No objects found for: (" + convertToString<int>(id) + "," + convertToString<int>(frame) + ")");
	}

	// Return the keys of an ObjectGroups structure
	list<int> pr::Results::getObjectGroupsKeys(const ObjectGroups& groups) const
	{
		list<int> keys;
		for(ObjectGroups::const_iterator it = groups.begin(); it != groups.end(); it++)
		{
			keys.push_back(it->first);
		}
		keys.sort();
		return keys;
	}
		
	int Results::size() const
	{
		// Initialize count
		int count = 0;
		// Count objects
		for(ObjectGroups::const_iterator g_it = id_map.begin(); g_it != id_map.end(); g_it++)
		{
			count += g_it->second.size();
		}
		// Return count
		return count;
	}
}
