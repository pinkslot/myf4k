#include <cv.h>
#include <highgui.h>
#include "fish_detection.h"
#include "contour_encoder.h"
#include <opencv_utils.h>
#include <iostream>

using namespace std;
using namespace cv;
using namespace db;

int main()
{
	try
	{
		Mat img = imread("test2.png");
		Mat gray;
		cvtColor(img, gray, CV_BGR2GRAY);
		cvb::CvBlobs blobs = computeBlobs(gray);
		cvb::CvBlob& blob = *(blobs.begin()->second);
		vector<Point> bb, cc, dec_bb, dec_cc, db_bb, db_cc;
		blobToBoundingBoxAndContour(blob, bb, cc);
		string bb_cc_str = ContourEncoder::encode(bb, cc);
		cout <<	"bb_cc_str: <start>" << bb_cc_str << "<end>" << endl;
		for(string::iterator it = bb_cc_str.begin(); it != bb_cc_str.end(); it++)
			cout << (int) *it << endl;
		ContourEncoder::decode(dec_bb, dec_cc, bb_cc_str);
		cvb::CvBlob* dec_blob = createBlob(dec_bb, dec_cc);
		showBlob("blob", blob);
		showBlob("dec_blob", *dec_blob);
		cout << "connecting" << endl;
		DBConnection conn("db_config.xml");
		FishDetection fd(&conn);
		/*fd.setField<int>("fish_id", -1);
		fd.setField<string>("video_id", "-2");
		fd.setField<int>("frame_id", -1);
		fd.setField<string>("bb_cc", bb_cc_str);
		fd.insertToDB();
		int id = fd.getField<int>("detection_id");
		cout << "ID: " << id << endl;*/
		/*fd.setField<int>("detection_id", 2067871);
		cout << "reading" << endl;
		fd.readFromDB();
		string db_bb_cc_str = fd.getField<string>("bb_cc");
		cout <<	"db bb_cc_str: <start>" << db_bb_cc_str << "<end>" << endl;
		ContourEncoder::decode(db_bb, db_cc, db_bb_cc_str);
		cvb::CvBlob* db_blob = createBlob(db_bb, db_cc);
		showBlob("db_blob", *db_blob);
		waitKey(0);*/
	}
	catch(MyException& e)
	{
		cout << e.what() << endl;
	}
}
