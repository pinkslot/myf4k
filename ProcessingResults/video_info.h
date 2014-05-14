// Information on videos
#ifndef VIDEO_INFO_H
#define VIDEO_INFO_H

struct VideoInfo
{
	std::string location;
	int cameraNumber;
	std::string date; // YYYY-MM-DD hh:mm
	int length;
	int frame_rate;
	int frame_height;
	int frame_width;
	int frame_depth;
	std::string encoding;
};

#endif
