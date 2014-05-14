// Read/write processing result/ground truth files

#ifndef XMLTDRESULTS_H
#define XMLTDRESULTS_H

#include <results.h>
#include <video_info.h>
#include <ticpp.h>

class XMLTDResults
{
	private:

	// Methods for each file version
	pr::Results readFileVersion0(ticpp::Document& doc);
	pr::Results readFileVersion1(ticpp::Document& doc);
	pr::Results readFileVersion2(ticpp::Document& doc, bool fish_only);

	public:
	
	// Constructor
	XMLTDResults() {}

	// Destructor
	virtual ~XMLTDResults() {}

	// Load from XML file
	pr::Results readFile(std::string file_path, bool fish_only = true);

	// Removes contour data and writes to file
	void stripContourData(std::string file_in, std::string file_out);

	// Write to XML file
	void writeFile(std::string file_path, const pr::Results& results, const VideoInfo& video_info);
	inline void writeFile(std::string file_path, const pr::Results& results) { writeFile(file_path, results, VideoInfo()); }
};

#endif
