#include <cv.h>
#include <cvblob.h>
#include <opencv_utils.h>
#include <highgui.h>
#include <iostream>
#include "tracking_evaluator.h"
#include <histogram.h>

using namespace cv;
using namespace std;

int main()
{
	TrackingEvaluator eval;
	//cout << eval.computeHistogramDiffScore(h1, h2);
	return 0;
	Mat img_in = imread("test.bmp");
	Mat img;
	if(img_in.channels() == 3)
		cvtColor(img_in, img, CV_BGR2GRAY);
	else
		img = img_in;
	IplImage img_ipl = img;
	IplImage* img_out_ipl = cvCreateImage(cvSize(img.cols, img.rows), IPL_DEPTH_LABEL, 1);
	cvb::CvBlobs blobs;
        cvb::cvLabel(&img_ipl, img_out_ipl, blobs);
	cvb::CvBlob& blob = *(blobs.begin()->second);
	blob.maxx = blob.maxx + 5;
	blob.maxy = blob.maxy + 5;
	Mat drawn = drawBlob(blob, false);
	Mat drawn3;
	cvtColor(drawn, drawn3, CV_GRAY2BGR);
	//vector<pair<Point, Point> > sampled = eval.sampleContour(blob);//, drawn3);
	//for(vector<pair<Point, Point> >::iterator it=sampled.begin(); it != sampled.end(); it++)
	//{
	//	line(drawn, it->first, it->second, CV_RGB(255,255,255));
	//}
	imshow("win", drawn3);
	waitKey(0);
	/*Point2f p1(5, 5);
	Point2f p2(45, 20);
	Point2f p0(80,80);
	cout << p2.y << " " << p1.y << " " << p2.x << " " << p1.x << endl;
	float m1 = (p2.y - p1.y)/(p2.x - p1.x);
	float m = -1/m1;
	float q = -m*p0.x + p0.y;
	cout << "m: " << m << "; q: " << q << endl;
	float l = 10;
	float k = l/sqrt(m*m+1);
	float xe1 = p0.x - k;
	float xe2 = p0.x + k;
	Point pe1(xe1, m*xe1+q);
	Point pe2(xe2, m*xe2+q);
	namedWindow("win");
	Mat frame = Mat::zeros(100, 100, CV_8UC3);
	circle(frame, p1, 3, CV_RGB(255,0,0));
	circle(frame, p2, 3, CV_RGB(0,255,0));
	circle(frame, p0, 3, CV_RGB(255,0,255));
	circle(frame, pe1, 3, CV_RGB(255,255,255));
	circle(frame, pe2, 3, CV_RGB(0,0,255));
	imshow("win", frame);
	waitKey(0);*/
}
