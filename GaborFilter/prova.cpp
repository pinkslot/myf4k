#include <cv.h>
#include <highgui.h>
#include <cmath>
#include <iostream>
#include "gabor_filter.h"

using namespace std;
using namespace cv;

int main()
{
	Mat img_rgb = imread("test_gabor.png");
	//Mat img_rgb = imread("img.bmp");
	Mat img;
	cvtColor(img_rgb, img, CV_RGB2GRAY);
	double lambda = 2;
	double theta = 0;
	double bandwidth = 1;
	double gamma = 1;
	Mat g_r = GaborFilter::createFilter(lambda, theta, 0, bandwidth, gamma, 3);
	cout << g_r.rows << " " << g_r.cols << endl;
	cout << g_r << endl;
	return 0;
	double min_val, max_val;
	minMaxLoc(g_r, &min_val, &max_val);
	min_val = abs(min_val);
	max_val = abs(max_val);
	max_val = (max_val >= min_val ? max_val : min_val);
	Mat g_hsv(g_r.rows, g_r.cols, CV_32FC3);
	for(int i=0; i<g_r.rows; i++)
	{
		for(int j=0; j<g_r.cols; j++)
		{
			float x = g_r.at<float>(i,j);
			float h = x*160.0/max_val + -160.0;
			g_hsv.at<Vec3f>(i,j) = Vec3f(h, 1, 1);
		}
	}
	Mat g_rgb; cvtColor(g_hsv, g_rgb, CV_HSV2BGR);
	//cout << g_r << endl;
	namedWindow("g");
	imshow("g", g_rgb);
	waitKey(0);
	Mat g_i = GaborFilter::createFilter(lambda, theta, M_PI/2, bandwidth, gamma);
	Mat filter_out = GaborFilter::applyFilter(img, g_r, g_i);
	//int channels = img.channels();
	//Mat* planes = new Mat[channels];
	//split(filter_out, planes);
	namedWindow("img");
	namedWindow("filtered");
	//namedWindow("plane_0");
	//namedWindow("plane_1");
	//namedWindow("plane_2");
	//imshow("img", img);
	//imshow("filtered", filter_out);
	//imshow("plane_0", planes[0]);
	//imshow("plane_1", planes[1]);
	//imshow("plane_2", planes[2]);
	//waitKey(0);
	Mat g_write;
	g_rgb.convertTo(g_write, CV_8U, 255);
	imwrite("gabor.png", g_write);
}
