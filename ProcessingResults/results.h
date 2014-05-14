// Output results of detection/tracking
// TODO improve memory usage
// TODO iterators

#ifndef PR_RESULTS_H
#define PR_RESULTS_H

#include "object.h"
#include "keyframes.h"
#include <map>
#include <list>

namespace pr
{

	typedef std::list<Object> ObjectList;
	typedef std::map<int, ObjectList> ObjectGroups;

	class Results
	{
		private:
		
		// Map between frame number and objects
		ObjectGroups frame_map;

		// Map between object id and objects
		ObjectGroups id_map;

		// Return the keys of an ObjectGroups structure
		std::list<int> getObjectGroupsKeys(const ObjectGroups& groups) const;

		// Keyframes
		KeyFrames keyframes;

		public:

		// Get keyframe reference
		inline KeyFrames& getKeyFrames() { return keyframes; }
		inline const KeyFrames& getKeyFrames() const { return keyframes; }

		// Add an object to the result set
		inline void addObject(const Object& o) { frame_map[o.getFrameNumber()].push_back(o); id_map[o.getId()].push_back(o); }
		
		// Add objects to the result set
		inline void addObjects(const ObjectList& os) { for(ObjectList::const_iterator o_it = os.begin(); o_it != os.end(); o_it++) addObject(*o_it); }

		// Get all objects in a frame
		ObjectList getObjectsByFrame(int frame_number) const;

		// Get all objects by id
		ObjectList getObjectsById(int id) const;
		
		// Get object by id and frame number
		Object getObject(int id, int frame) const;

		// Get frame numbers
		inline std::list<int> frameList() const { return getObjectGroupsKeys(frame_map); }

		// Get id numbers
		inline std::list<int> idList() const { return getObjectGroupsKeys(id_map); }
		
		// Get total number of objects
		int size() const;
	};

};

#endif
