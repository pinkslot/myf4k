// Read/write processing result/ground truth files

#include "xmltdresults.h"
#include "keyframes.h"
#include <shell_utils.h>
#include <opencv_utils.h>
#include <log.h>
#include <sstream>
#include <string_utils.h>

using namespace ticpp;
using namespace std;
using namespace pr;
using namespace cv;

Results XMLTDResults::readFileVersion0(Document& doc)
{
	// Initialize result
	Results results;
	// Read XML
	try
	{
		// Read all frames
		Node* video = doc.FirstChild("video");
		Iterator<Element> frame("frame");
		for(frame = frame.begin(video); frame != frame.end(); frame++)
		{
			int frame_id = frame->GetAttribute<int>("id"); 
			// Read all frame objects
			Iterator<Element> object("object");
			for(object = object.begin(&*frame); object != object.end(); object++)
			{
				try
				{
					//if(frame_id > 200) continue;
					//{
					//	cout << object->GetAttribute<int>("trackingId") << endl;
					//}
					// Initialize element
					Object pr_object;
					pr_object.setFrameNumber(frame_id);
					pr_object.setId(object->GetAttribute<int>("trackingId"));
					pr_object.setType("fish");
					float tmp_certainty;
					object->GetAttributeOrDefault<float>("detectionCertainty", &tmp_certainty, -1);
					pr_object.setDetectionCertainty(tmp_certainty);
					object->GetAttributeOrDefault<float>("trackingCertainty", &tmp_certainty, -1);
					pr_object.setTrackingCertainty(tmp_certainty);
					// Read bounding box and contour
					Element* bounding_box = object->FirstChildElement("bbox");
					Element* contour = object->FirstChildElement("contour");
					// Read bounding box and compute top-left corner
					//vector<Point> pr_bounding_box;
					Iterator<Element> point("point");
					int offset_x = INT_MAX;
					int offset_y = INT_MAX;
					int max_x = 0;
					int max_y = 0;
					for(point = point.begin(bounding_box); point != point.end(); point++)
					{
						// Read coordinates
						int point_x = point->GetAttribute<int>("x");
						int point_y = point->GetAttribute<int>("y");
						// Add to bounding box
						//pr_bounding_box.push_back(Point(point_x, point_y));
						// Check offsets
						if(point_x < offset_x)	offset_x = point_x;
						if(point_y < offset_y)	offset_y = point_y;
						// Check max coordinates
						if(point_x > max_x)	max_x = point_x;
						if(point_y > max_y)	max_y = point_y;
					}
					//pr_object.setBoundingBox(pr_bounding_box);
					// Read contour
					vector<Point> pr_contour;
					for(point = point.begin(contour); point != point.end(); point++)
					{
						Point p;
						p.x = offset_x + point->GetAttribute<int>("x");
						p.y = offset_y + point->GetAttribute<int>("y");
						// Check bounding box limits
						if(p.x > max_x || p.y > max_y)
						{
							Log::warning(0) << "Error reading object from XML data (tracking id: " << object->GetAttribute<int>("trackingId") << ", detection id: " << object->GetAttribute<int>("detectionId") << "): invalid contour coordinate (" << p.x << ", " << p.y << ")." << endl;
						}
						else
						{
							// Add to contour
							pr_contour.push_back(p);
						}
					}
					pr_object.setContour(pr_contour);
					// Add object to results
					results.addObject(pr_object);
				}
				catch(MyException& e)
				{
					Log::warning(0) << "Error reading object from XML data (tracking id: " << object->GetAttribute<int>("trackingId") << ", detection id: " << object->GetAttribute<int>("detectionId") << "): " << e.what() << endl;
				}
			}
		}
	}
	catch(ticpp::Exception& ex)
	{
		stringstream ss;
		ss << "XML error: " << ex.what() << endl;
		throw MyException(ss.str());
	}
	// Return results
	return results;
}

Results XMLTDResults::readFileVersion1(Document& doc)
{
	// Initialize result
	Results results;
	// Read XML
	try
	{
		// Read all frames
		Node* video = doc.FirstChild("video");
		Iterator<Element> frame("frame");
		for(frame = frame.begin(video); frame != frame.end(); frame++)
		{
			int frame_id = frame->GetAttribute<int>("id"); 
			// Read all frame objects
			Iterator<Element> object("object");
			for(object = object.begin(&*frame); object != object.end(); object++)
			{
				try
				{
					// Initialize element
					Object pr_object;
					pr_object.setFrameNumber(frame_id);
					pr_object.setId(object->GetAttribute<int>("trackingId"));
					float tmp_certainty;
					object->GetAttributeOrDefault<float>("detectionCertainty", &tmp_certainty, -1);
					pr_object.setDetectionCertainty(tmp_certainty);
					object->GetAttributeOrDefault<float>("trackingCertainty", &tmp_certainty, -1);
					pr_object.setTrackingCertainty(tmp_certainty);
					// Read bounding box and contour
					//Element* bounding_box = object->FirstChildElement("bbox");
					Element* contour = object->FirstChildElement("contour");
					// Parse bounding box
					//vector<Point> pr_bounding_box;
					/*int min_x = INT_MAX;
					int min_y = INT_MAX;
					int max_x = 0;
					int max_y = 0;
					vector<string> xy_list = splitString(bounding_box->GetAttribute<string>("points"), ',');
					for(vector<string>::iterator xy_it = xy_list.begin(); xy_it != xy_list.end(); xy_it++)
					{
						// Read coordinates
						vector<string> xy = splitString(*xy_it, ' ');
						if(xy.size() != 2)
						{
							throw MyException("Size of 'x y' pair bigger than 2: " + *xy_it);
						}
						int x = convertFromString<int>(xy[0]);
						int y = convertFromString<int>(xy[1]);
						// Check min/max
						if(x < min_x)	min_x = x;
						if(x > max_x)	max_x = x;
						if(y < min_y)	min_y = y;
						if(y > max_y)	max_y = y;
						// Add to contour
						//pr_bounding_box.push_back(Point(x, y));
					}
					//pr_object.setBoundingBox(pr_bounding_box);*/
					// Parse contour
					vector<Point> pr_contour;
					//min_x = INT_MAX;
					//min_y = INT_MAX;
					//max_x = 0;
					//max_y = 0;
					vector<string> xy_list = splitString(contour->GetAttribute<string>("points"), ',');
					for(vector<string>::iterator xy_it = xy_list.begin(); xy_it != xy_list.end(); xy_it++)
					{
						// Read coordinates
						vector<string> xy = splitString(*xy_it, ' ');
						if(xy.size() != 2)
						{
							throw MyException("Size of 'x y' pair bigger than 2: " + *xy_it);
						}
						int x = convertFromString<int>(xy[0]);
						int y = convertFromString<int>(xy[1]);
						// Check min/max
						//if(x < min_x)	min_x = x;
						//if(x > max_x)	max_x = x;
						//if(y < min_y)	min_y = y;
						//if(y > max_y)	max_y = y;
						// Add to contour
						pr_contour.push_back(Point(x, y));
					}
					pr_object.setContour(pr_contour);
					// Add object to results
					results.addObject(pr_object);
				}
				catch(MyException& e)
				{
					Log::warning(0) << "Error reading object from XML data (tracking id: " << object->GetAttribute<int>("trackingId") << ", detection id: " << object->GetAttribute<int>("detectionId") << "): " << e.what() << endl;
				}
			}
		}
	}
	catch(ticpp::Exception& ex)
	{
		stringstream ss;
		ss << "XML error: " << ex.what() << endl;
		throw MyException(ss.str());
	}
	// Return results
	return results;
}

Results XMLTDResults::readFileVersion2(Document& doc, bool fish_only)
{
	// Initialize result
	Results results;
	// Map tracking ID to object types
	map<int, string> id_type_map;
	// Read XML
	try
	{
		// Read all frames
		Node* video = doc.FirstChild("video");
		// Initialize keyframe structure
		KeyFrames keyframes;
		// Read keyframes
		Node* keyframes_node = video->FirstChild("keyFrames", false);
		if(keyframes_node != NULL)
		{
			Iterator<Element> keyframe_it("keyFrame");
			for(keyframe_it = keyframe_it.begin(keyframes_node); keyframe_it != keyframe_it.end(); keyframe_it++)
			{
				// Get start and end values
				int start = keyframe_it->GetAttribute<int>("start");
				int end = keyframe_it->GetAttribute<int>("end", -1);
				// Check end
				if(end == -1)
				{
					// Single frame, set end = start
					end = start;
				}
				// Get label
				string label = keyframe_it->GetTextOrDefault("");
				// Add to keyframes
				keyframes.add(start, end, label);
			}
		}
		// Save keyframes
		results.getKeyFrames() = keyframes;
		// Read frames
		Iterator<Element> frame("frame");
		for(frame = frame.begin(video); frame != frame.end(); frame++)
		{
			int frame_id = frame->GetAttribute<int>("id"); 
			// Read all frame objects
			Iterator<Element> object("object");
			for(object = object.begin(&*frame); object != object.end(); object++)
			{
				// Read tracking ID
				int tracking_id = object->GetAttribute<int>("trackingId");
				// Initialize the object type from the XML attribute (and convert it to lowercase)
				string type = object->GetAttributeOrDefault("objectType", "fish");
				if(type == "")	type = "fish";
				transform(type.begin(), type.end(), type.begin(), ::tolower);
				// Check if we actually already had a type for this ID
				if(id_type_map.find(tracking_id) != id_type_map.end())
				{
					// Check if types are different
					if(id_type_map[tracking_id] != type)
					{
						Log::w() << ShellUtils::RED << "XMLTDResults: object " << tracking_id << " has multiple types." << ShellUtils::NORMAL << endl;
					}
					// Set the type to the one in the map
					type = id_type_map[tracking_id];
				}
				else
				{
					// Set the type in the map
					id_type_map[tracking_id] = type;
				}
				// Check if we want to select only fish
				if(fish_only && type != "fish")
				{
					Log::d() << "Skipping object " << tracking_id << ", detection " << object->GetAttributeOrDefault("detectionId", "undefined") << " with non-fish type: " << type << endl;
					continue;
				}
				// Build object
				try
				{
					// Initialize element
					Object pr_object;
					pr_object.setFrameNumber(frame_id);
					pr_object.setId(tracking_id);
					//Log::d() << "XML, setting type for (" << tracking_id << "," << frame_id << "): " << type << endl;
					pr_object.setType(type);
					//Log::d() << "reading type: " << pr_object.getType() << endl;
					// Set optional
					pr_object.setOptional(convertFromString<bool>(object->GetAttributeOrDefault("optional", "0")));
					//cout << "optional: " << object->GetAttributeOrDefault("optional", "0") << ", " << pr_object.getOptional() << endl;
					// Set certainty (if defined)
					float tmp_certainty;
					object->GetAttributeOrDefault<float>("detectionCertainty", &tmp_certainty, -1);
					pr_object.setDetectionCertainty(tmp_certainty);
					object->GetAttributeOrDefault<float>("trackingCertainty", &tmp_certainty, -1);
					pr_object.setTrackingCertainty(tmp_certainty);
					// Read contour or bounding box
					Element* contour = object->FirstChildElement("contour", false);
					Element* bbox = object->FirstChildElement("bbox", false);
					string points = "";
					// Check contour
					if(contour != NULL)
					{
						// Read contour points
						points = contour->GetTextOrDefault("");
					}
					// Check contour output
					if(points.size() == 0)
					{
						// Check bounding box
						if(bbox != NULL)
						{
							// Read bounding box points
							points = bbox->GetTextOrDefault("");
						}
					}
					// Check final points
					if(points.size() == 0)
					{
						throw MyException("No valid bounding box or contour found.");
					}
					// Parse contour
					vector<Point> pr_contour;
					vector<string> xy_list = splitString(points, ',');
					for(vector<string>::iterator xy_it = xy_list.begin(); xy_it != xy_list.end(); xy_it++)
					{
						// Read coordinates
						vector<string> xy = splitString(*xy_it, ' ');
						if(xy.size() != 2)
						{
							throw MyException("XMLTDResults: size of 'x y' pair bigger than 2: " + *xy_it);
						}
						int x = convertFromString<int>(xy[0]);
						int y = convertFromString<int>(xy[1]);
						// Add to contour
						pr_contour.push_back(Point(x, y));
					}
					// Fix contour
					pr_contour = fixContour(pr_contour);
					// Check minimum number of points
					if(pr_contour.size() < 3)
					{
						throw MyException("Not enough contour points");
					}
					// Set contour
					pr_object.setContour(pr_contour);
					// Add object to results
					results.addObject(pr_object);
				}
				catch(MyException& e)
				{
					Log::warning(0) << "Error reading object from XML data (tracking id: " << object->GetAttribute<int>("trackingId") << ", detection id: " << object->GetAttribute<int>("detectionId") << "): " << e.what() << endl;
				}
			}
		}
	}
	catch(ticpp::Exception& ex)
	{
		stringstream ss;
		ss << "XML error: " << ex.what() << endl;
		throw MyException(ss.str());
	}
	// Return results
	return results;
}
	
// Load from XML file
Results XMLTDResults::readFile(string file_path, bool fish_only)
{
	// Read XML
	try
	{
		Document doc(file_path);
		doc.LoadFile();
		// Check version
		Element* video = doc.FirstChildElement("video");
		string version;
	       	video->GetAttributeOrDefault("version", &version, "0");
		if(version == "0")
		{
			// Version 0
			// <point x= y=>...
			return readFileVersion0(doc);
		}
		else if(version == "1")
		{
			// Version 1
			// <contour points='0 0,1 1,...'>
			return readFileVersion1(doc);
		}
		else if(version == "2")
		{
			// Version 1
			// <keyframes>...</keyframes>
			// <contour>0 0,1 1,...</contour>
			return readFileVersion2(doc, fish_only);
		}
		else
		{
			throw MyException("Invalid XML version: " + version);
		}
	}
	catch(ticpp::Exception& ex)
	{
		stringstream ss;
		ss << "XML error: " << ex.what() << endl;
		throw MyException(ss.str());
	}
}
	
// Removes contour data and writes to file
void XMLTDResults::stripContourData(string file_in, string file_out)
{
	// Read XML
	try
	{
		Document doc(file_in);
		doc.LoadFile();
		// Read all frames
		Node* video = doc.FirstChild("video");
		Iterator<Element> frame("frame");
		for(frame = frame.begin(video); frame != frame.end(); frame++)
		{
			//int frame_id = frame->GetAttribute<int>("id"); 
			// Read all frame objects
			Iterator<Element> object("object");
			for(object = object.begin(&*frame); object != object.end(); object++)
			{
				try
				{
					// Get contour
					Element* contour = object->FirstChildElement("contour");
					// Clear contour
					contour->Clear();
					// Add 4 dummy elements
					Element e1("point"); e1.SetAttribute("x", "0"); e1.SetAttribute("y", "0"); contour->LinkEndChild(&e1);
					Element e2("point"); e2.SetAttribute("x", "0"); e2.SetAttribute("y", "0"); contour->LinkEndChild(&e2);
					Element e3("point"); e3.SetAttribute("x", "0"); e3.SetAttribute("y", "0"); contour->LinkEndChild(&e3);
					Element e4("point"); e4.SetAttribute("x", "0"); e4.SetAttribute("y", "0"); contour->LinkEndChild(&e4);
				}
				catch(MyException& e)
				{
					Log::warning(0) << "Error reading object from XML data (tracking id: " << object->GetAttribute<int>("trackingId") << ", detection id: " << object->GetAttribute<int>("detectionId") << "): " << e.what() << endl;
				}
			}
		}
		// Save output file
		doc.SaveFile(file_out);
	}
	catch(ticpp::Exception& ex)
	{
		stringstream ss;
		ss << "XML error: " << ex.what() << endl;
		throw MyException(ss.str());
	}
}

// Write to XML file
void XMLTDResults::writeFile(string file_path, const Results& results, const VideoInfo& video_info)
{
	// Counter for detection id's
	long int next_detection_id = 0;
	// List of pointers to delete
	list<Element*> to_delete;
	try
	{
		// Create document
		Document doc(file_path);
		// Add XML declaration
		Declaration decl("1.0", "", "");
		doc.InsertEndChild(decl);
		// Create video element
		Element video("video");
		// Add video information
		video.SetAttribute("name", "");
		video.SetAttribute("location", video_info.location);
		video.SetAttribute("camera", video_info.cameraNumber);
		video.SetAttribute("date", video_info.date);
		video.SetAttribute("fps", video_info.frame_rate);
		video.SetAttribute("height", video_info.frame_height);
		video.SetAttribute("width", video_info.frame_width);
		video.SetAttribute("version", "2");
		// Add video to document
		doc.LinkEndChild(&video);
		// Create keyframe element
		Element keyframes("keyFrames");
		// Add keyframe intervals
		list<KeyFrameInterval> kf_intervals = results.getKeyFrames().getIntervals();
		for(list<KeyFrameInterval>::iterator k_it = kf_intervals.begin(); k_it != kf_intervals.end(); k_it++)
		{
			// Get start and end values
			int start = k_it->start();
			int end = k_it->end();
			// Get label
			string label = k_it->label;
			// Create element
			Element* keyframe = new Element("keyFrame"); to_delete.push_back(keyframe);
			// Set attributes
			keyframe->SetAttribute("start", start);
			keyframe->SetAttribute("end", end);
			keyframe->SetText(label);
			// Add element to the parent node
			keyframes.LinkEndChild(keyframe);
		}
		// Add keyframe section
		video.LinkEndChild(&keyframes);
		// Get frame list
		list<int> frame_numbers = results.frameList();
		// Go through all frames
		for(list<int>::iterator f_it = frame_numbers.begin(); f_it != frame_numbers.end(); f_it++)
		{
			// Read frame number
			int frame_num = *f_it;
			// Add element
			Element* frame = new Element("frame"); to_delete.push_back(frame);
			frame->SetAttribute("id", frame_num);
			video.LinkEndChild(frame);
			// Get objects
			ObjectList objects = results.getObjectsByFrame(frame_num);
			// Go through all objects
			for(ObjectList::iterator o_it = objects.begin(); o_it != objects.end(); o_it++)
			{
				// Read object
				Object pr_object = *o_it;
				// Create element
				Element* object = new Element("object"); to_delete.push_back(object);
				object->SetAttribute("trackingId", pr_object.getId());
				object->SetAttribute("detectionId", next_detection_id++);
				if(pr_object.getDetectionCertainty() > -1)
					object->SetAttribute<float>("detectionCertainty", pr_object.getDetectionCertainty());
				if(pr_object.getTrackingCertainty() > -1)
					object->SetAttribute<float>("trackingCertainty", pr_object.getTrackingCertainty());
				string type = pr_object.getType();
				if(type == "")	type = "fish";
				object->SetAttribute("objectType", type);
				object->SetAttribute("optional", pr_object.getOptional());
				frame->LinkEndChild(object);
				// Create contour element
				Element* contour = new Element("contour"); to_delete.push_back(contour);
				object->LinkEndChild(contour);
				// Read contour
				vector<Point> pr_contour = pr_object.getContour();
				// Add contour points 
				stringstream contour_points;
				if(pr_contour.size() > 0)
				{
					vector<Point>::iterator p_it = pr_contour.begin();
					contour_points << p_it->x << " " << p_it->y;
					for(p_it++; p_it != pr_contour.end(); p_it++)
					{
						//cout << "contour: " << p_it->x << " " << p_it->y << endl;
						contour_points << "," << p_it->x << " " << p_it->y;
					}
				}
				contour->SetText(contour_points.str());
			}
		}
		// Write document to file
		doc.SaveFile();
	}
	catch(ticpp::Exception& ex)
	{
		stringstream ss;
		ss << "XML error: " << ex.what() << endl;
		throw MyException(ss.str());
	}
	// Delete pointers
	for(list<Element*>::iterator it = to_delete.begin(); it != to_delete.end(); it++)
	{
		delete *it;
	}
}
