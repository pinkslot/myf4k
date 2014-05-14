#include "histogram.h"
#include <iostream>

using namespace std;

int main()
{
	Mat img_mat = Mat::zeros(3, 3, CV_8UC1);
	Mat_<uchar>& img = (Mat_<uchar>&) img_mat;
	img << 0, 253, 254, 254, 254, 1, 1, 1, 255;
	cout << "img = " << endl << img << endl << endl;
	Histogram histogram(img);
	//Mat hist = histogram.getHistogram();
	cout << "hist = " << endl << histogram << endl << endl;
	int m, M;
	//histogram.trim(0.25, &m, &M);
	histogram.equalize();
	//hist = histogram.getHistogram();
	cout << "hist = " << endl << histogram << endl << endl;
	//cout << m << " " << M << endl;
	//cout << "mean = " << histogram.mean() << endl << endl;
	//double *moments = new double[3];
	//histogram.moments(4, moments);
	//cout << "moment 2 = " << moments[0] << endl;
	//cout << "moment 3 = " << moments[1] << endl;
	//cout << "moment 4 = " << moments[2] << endl;
	//cout << "--------" << endl;
	//cout << "moment 0 = " << histogram.moment(0) << endl;
	//cout << "moment 1 = " << histogram.moment(1) << endl;
	//cout << "moment 2 = " << histogram.moment(2) << endl;
	//cout << "moment 3 = " << histogram.moment(3) << endl;
	//cout << "moment 4 = " << histogram.moment(4) << endl;
}
