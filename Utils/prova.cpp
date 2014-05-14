#include "opencv_utils.h"
#include "machine_utils.h"
#include "time_utils.h"
#include "string_utils.h"
#include "math_utils.h"
#include "feature_extractor_set.h"
#include "xy_feature_extractor.h"
#include "rgb_feature_extractor.h"
#include "hue_feature_extractor.h"
#include "deriv_feature_extractor.h"
#include "log.h"
#include <iostream>
#include <unistd.h>
#include <highgui.h>

using namespace std;
using namespace cv;
using namespace alg;

int main(int argc, char** argv)
{
	// Initialize log
	Log::setDebugLogStream(cout);
	Log::setInfoLogStream(cout);
	Log::setWarningLogStream(cout);
	Log::setErrorLogStream(cerr);
	// Test motion vector
	Mat img_prev = imread("/home/simone/Matlab/certainty/frame_20.jpg");
	Mat img_next = imread("/home/simone/Matlab/certainty/frame_21.jpg");
	int rows = img_prev.rows;
	int cols = img_prev.cols;
	Rect rect(0, 0, 320, 240);
	// Build input points vector
	vector<Point2f> prev_points;
	for(int x=rect.x; x<rect.x+rect.width; x++)
	{
		for(int y=rect.y; y<rect.y+rect.height; y++)
		{
			prev_points.push_back(Point2f(x,y));
		}
	}
	//prev_points.push_back(Point2f(240,40));
	// Compute optical flow
	vector<Point2f> next_points;
	vector<uchar> status;
	vector<float> err;
	calcOpticalFlowPyrLK(img_prev, img_next, prev_points, next_points, status, err, Size(3, 3), 3, TermCriteria(TermCriteria::MAX_ITER, 5, 0), 0);
	//cout << next_points[0].x << " " << next_points[0].y << endl;
	// Create output matrix
	Mat motion_vector = Mat::zeros(rows, cols, CV_64FC4);
	for(unsigned int i=0; i<prev_points.size(); i++)
	{
		const Point2f& prev_pt = prev_points[i];
		const Point2f& next_pt = next_points[i];
		// Compute modulus and phase
		double modulus = sqrt((prev_pt.x - next_pt.x)*(prev_pt.x - next_pt.x) + (prev_pt.y - next_pt.y)*(prev_pt.y - next_pt.y));
		double phase = fastAtan2(-next_pt.y + prev_pt.y, next_pt.x - prev_pt.x);
		// Compute x and y offsets
		double delta_x = next_pt.x - prev_pt.x;
		double delta_y = next_pt.y - prev_pt.y;
		// Set corresponding pixel in the motion vector
		motion_vector.at<Vec4d>(prev_pt.y, prev_pt.x)[0] = modulus;
		motion_vector.at<Vec4d>(prev_pt.y, prev_pt.x)[1] = phase;
		motion_vector.at<Vec4d>(prev_pt.y, prev_pt.x)[2] = delta_x;
		motion_vector.at<Vec4d>(prev_pt.y, prev_pt.x)[3] = delta_y;
	}
	// Split channels
	Mat channels[4];
	split(motion_vector, channels);
	// Get min and max
	double min_val;
	double max_val;
	minMaxLoc(channels[0], &min_val, &max_val);
	cout << "min: " << min_val << ", max: " << max_val << endl;
	imshow("mvm", channels[0]/max_val);
	waitKey(0);
	// Test superpixels straddling
	/*Mat frame = imread("/home/simone/Matlab/certainty/imgs/good/frame_5.png");
	Mat mask = imread("/home/simone/Matlab/certainty/imgs/good/mask_5.png");
	Mat seg;
	int nc;
	segmentSuperpixels(frame, seg, nc, 500, 25);
	float sss = superpixelsStraddlingScore(seg, mask);
	cout << sss << endl;
	imshow("frame", frame);
	imshow("mask", mask);
	imshow("seg", seg);
	waitKey(0);*/
	// Test contour
	/*vector<Point> c;
	c.push_back(Point(40,21));
	c.push_back(Point(39,21));
	c.push_back(Point(38,21));
	c.push_back(Point(37,21));
	c.push_back(Point(36,21));
	c.push_back(Point(35,21));
	c.push_back(Point(34,21));
	c.push_back(Point(33,21));
	c.push_back(Point(33,20));
	c.push_back(Point(33,19));
	c.push_back(Point(33,18));
	c.push_back(Point(33,17));
	c.push_back(Point(33,16));
	c.push_back(Point(33,15));
	c.push_back(Point(33,14));
	c.push_back(Point(33,13));
	c.push_back(Point(33,12));
	c.push_back(Point(33,11));
	c.push_back(Point(33,10));
	c.push_back(Point(33,9));
	c.push_back(Point(33,8));
	c.push_back(Point(33,7));
	c.push_back(Point(32,7));
	c.push_back(Point(32,6));
	c.push_back(Point(31,5));
	c.push_back(Point(30,4));
	c.push_back(Point(30,3));
	c.push_back(Point(29,3));
	c.push_back(Point(29,2));
	c.push_back(Point(28,2));
	c.push_back(Point(27,2));
	c.push_back(Point(26,1));
	c.push_back(Point(25,1));
	c.push_back(Point(24,0));
	c.push_back(Point(23,0));
	c.push_back(Point(22,0));
	c.push_back(Point(21,0));
	c.push_back(Point(20,0));
	c.push_back(Point(19,0));
	c.push_back(Point(18,0));
	c.push_back(Point(18,1));
	c.push_back(Point(17,1));
	c.push_back(Point(17,2));
	c.push_back(Point(16,2));
	c.push_back(Point(15,2));
	c.push_back(Point(14,2));
	c.push_back(Point(13,3));
	c.push_back(Point(12,3));
	c.push_back(Point(11,4));
	c.push_back(Point(11,5));
	c.push_back(Point(10,5));
	c.push_back(Point(9,6));
	c.push_back(Point(9,7));
	c.push_back(Point(9,8));
	c.push_back(Point(8,9));
	c.push_back(Point(7,9));
	c.push_back(Point(6,10));
	c.push_back(Point(6,11));
	c.push_back(Point(6,12));
	c.push_back(Point(5,12));
	c.push_back(Point(5,13));
	c.push_back(Point(4,13));
	c.push_back(Point(3,14));
	c.push_back(Point(2,15));
	c.push_back(Point(2,16));
	c.push_back(Point(1,16));
	c.push_back(Point(0,16));
	c.push_back(Point(0,17));
	c.push_back(Point(0,18));
	c.push_back(Point(0,19));
	c.push_back(Point(0,20));
	c.push_back(Point(0,21));
	c.push_back(Point(0,22));
	c.push_back(Point(1,22));
	c.push_back(Point(1,23));
	c.push_back(Point(1,24));
	c.push_back(Point(1,25));
	c.push_back(Point(2,25));
	c.push_back(Point(3,25));
	c.push_back(Point(3,26));
	c.push_back(Point(3,27));
	c.push_back(Point(4,27));
	c.push_back(Point(4,28));
	c.push_back(Point(4,29));
	c.push_back(Point(5,29));
	c.push_back(Point(5,30));
	c.push_back(Point(5,31));
	c.push_back(Point(5,32));
	c.push_back(Point(6,32));
	c.push_back(Point(6,33));
	c.push_back(Point(6,34));
	c.push_back(Point(6,35));
	c.push_back(Point(6,36));
	c.push_back(Point(6,37));
	c.push_back(Point(6,38));
	c.push_back(Point(6,39));
	c.push_back(Point(6,40));
	c.push_back(Point(6,41));
	c.push_back(Point(7,41));
	c.push_back(Point(7,42));
	c.push_back(Point(7,43));
	c.push_back(Point(8,44));
	c.push_back(Point(8,45));
	c.push_back(Point(9,45));
	c.push_back(Point(10,45));
	c.push_back(Point(11,45));
	c.push_back(Point(12,45));
	c.push_back(Point(13,45));
	c.push_back(Point(14,45));
	c.push_back(Point(15,45));
	c.push_back(Point(16,45));
	c.push_back(Point(17,45));
	c.push_back(Point(18,45));
	c.push_back(Point(19,45));
	c.push_back(Point(20,45));
	c.push_back(Point(21,45));
	c.push_back(Point(22,45));
	c.push_back(Point(23,45));
	c.push_back(Point(23,44));
	c.push_back(Point(22,43));
	c.push_back(Point(21,43));
	c.push_back(Point(21,42));
	c.push_back(Point(20,41));
	c.push_back(Point(19,41));
	c.push_back(Point(19,40));
	c.push_back(Point(19,39));
	c.push_back(Point(20,39));
	c.push_back(Point(21,39));
	c.push_back(Point(22,39));
	c.push_back(Point(23,39));
	c.push_back(Point(23,40));
	c.push_back(Point(24,40));
	c.push_back(Point(25,40));
	c.push_back(Point(26,41));
	c.push_back(Point(27,41));
	c.push_back(Point(28,41));
	c.push_back(Point(29,41));
	c.push_back(Point(30,41));
	c.push_back(Point(31,41));
	c.push_back(Point(31,40));
	c.push_back(Point(31,39));
	c.push_back(Point(31,38));
	c.push_back(Point(31,37));
	c.push_back(Point(31,36));
	c.push_back(Point(31,35));
	c.push_back(Point(31,34));
	c.push_back(Point(31,33));
	c.push_back(Point(32,33));
	c.push_back(Point(33,33));
	c.push_back(Point(33,34));
	c.push_back(Point(34,34));
	c.push_back(Point(34,35));
	c.push_back(Point(34,36));
	c.push_back(Point(35,37));
	c.push_back(Point(36,37));
	c.push_back(Point(36,36));
	c.push_back(Point(36,35));
	c.push_back(Point(36,34));
	c.push_back(Point(36,33));
	c.push_back(Point(36,32));
	c.push_back(Point(36,31));
	c.push_back(Point(36,30));
	c.push_back(Point(36,29));
	c.push_back(Point(37,29));
	c.push_back(Point(37,28));
	c.push_back(Point(38,28));
	c.push_back(Point(39,27));
	c.push_back(Point(39,26));
	c.push_back(Point(39,25));
	c.push_back(Point(39,24));
	c.push_back(Point(40,24));
	c.push_back(Point(40,23));
	c.push_back(Point(40,22));
	c.push_back(Point(40,21));
	cvb::CvBlob* blob = createBlob(c);
	Mat mask = blobToMat(*blob, CV_POSITION_ORIGINAL, true, 200, 200);
	cout << boundaryComplexity(mask, c, 5, 8, 5) << endl;*/
	// Read images
	/*Log::i() << "Reading images." << endl;
	Mat fish[4];
	fish[0] = imread("fish_1.png");
	fish[1] = imread("fish_2.png");
	fish[2] = imread("fish_3.png");
	fish[3] = imread("fish_4.png");
	Mat other[4];
	other[0] = imread("other_1.png");
	other[1] = imread("other_2.png");
	other[2] = imread("other_3.png");
	other[3] = imread("other_4.png");
	Mat items[4];
	items[0] = imread("algae.png");
	items[1] = imread("bed.png");
	items[2] = imread("sea_clear.png");
	items[3] = imread("sea_dark.png");
	string item_names[4] = {"algae", "bed", "sea_clear", "sea_dark"};
	// Define feature extractor
	FeatureExtractorSet* extractor_set = new FeatureExtractorSet();
	extractor_set->addExtractor(new XYFeatureExtractor());
	extractor_set->addExtractor(new RGBFeatureExtractor());
	extractor_set->addExtractor(new HueFeatureExtractor());
	extractor_set->addExtractor(new DerivFeatureExtractor(1,0));
	extractor_set->addExtractor(new DerivFeatureExtractor(0,1));
	CovarianceFeatureExtractor* extractor = extractor_set;
	Log::i() << "Computing covariance matrices." << endl;
	// Compute covariance matrices
	Mat c_fish[4];
	for(int i=0; i<4; i++)	c_fish[i] = MathUtils::computeCovarianceMatrix(extractor->computeFeatureSet(fish[i]));
	Mat c_other[4];
	for(int i=0; i<4; i++)	c_other[i] = MathUtils::computeCovarianceMatrix(extractor->computeFeatureSet(other[i]));
	Mat c_items[4];
	for(int i=0; i<4; i++)	c_items[i] = MathUtils::computeCovarianceMatrix(extractor->computeFeatureSet(items[i]));*/
	// Test covariance mean
	/*Mat exp_a = MathUtils::exponentialMap(c_fish[0]);
	Mat exp_b = MathUtils::exponentialMap(c_fish[1]);
	Mat bch_a_b = c_fish[0] + c_fish[1] + 0.5*(exp_a*exp_b - exp_b*exp_a);
	Mat exp_bch_a_b = MathUtils::exponentialMap(bch_a_b);
	Log::d() << "a = " << endl << c_fish[0] << endl << endl;
	Log::d() << "b = " << endl << c_fish[1] << endl << endl;
	Log::d() << "exp(a) = " << endl << exp_a << endl << endl;
	Log::d() << "exp(b) = " << endl << exp_b << endl << endl;
	Log::d() << "exp(a)*exp(b) = " << endl << (exp_a*exp_b) << endl << endl;
	Log::d() << "BCH(a,b) = " << endl << bch_a_b << endl << endl;
	Log::d() << "exp(BCH(a,b)) = " << endl << (exp_bch_a_b) << endl << endl;
	exit(1);*/
	// Compute distances between fish_*
	/*Log::i() << "Distances between fish_*:" << endl;
	for(int i=0; i<4; i++) for(int j=0; j<4; j++) if(i < j) Log::i() << i << "-" << j << ": " << MathUtils::covarianceDistance(c_fish[i], c_fish[j]) << endl;
	// Compute distances between fish_* and other_*
	Log::i() << "Distances between fish_* and other_*:" << endl;
	for(int i=0; i<4; i++) for(int j=0; j<4; j++) Log::i() << i << "-" << j << ": " << MathUtils::covarianceDistance(c_fish[i], c_other[j]) << endl;
	// Compute distances between fish_* and items
	Log::i() << "Distances between fish_* and items:" << endl;
	for(int i=0; i<4; i++) for(int j=0; j<4; j++) Log::i() << i << "-" << item_names[j] << ": " << MathUtils::covarianceDistance(c_fish[i], c_items[j]) << endl;
	// Compute distances between other_* and items
	Log::i() << "Distances between other_* and items:" << endl;
	for(int i=0; i<4; i++) for(int j=0; j<4; j++) Log::i() << i << "-" << item_names[j] << ": " << MathUtils::covarianceDistance(c_other[i], c_items[j]) << endl;
	// Compute average covariance matrix for fish_*
	vector<const Mat*> c_vec;
	for(int i=0; i<4; i++) c_vec.push_back(&(c_fish[i]));
	//for(int i=0; i<4; i++) cout << "-----------------" << endl << c_fish[i] << endl << endl << *(c_vec[i]) << endl << "----------------" << endl;
	//Mat fish_mean = MathUtils::covarianceLieMeanTest(c_vec, Mat(), false);
	Mat fish_mean_1 = MathUtils::covarianceLieMean(c_vec, Mat(), false);
	Mat fish_mean_2 = MathUtils::covarianceLieMean(c_fish, 4, Mat(), false);
	//Log::i() << fish_mean_1 << endl << endl;
	//Log::i() << fish_mean_2 << endl << endl;
	Mat fish_mean = fish_mean_1;
	// Compute distances between fish_* and fish_mean
	Log::i() << "Distances between fish_* and fish_mean:" << endl;
	for(int i=0; i<4; i++) Log::i() << i << "-mean: " << MathUtils::covarianceDistance(c_fish[i], fish_mean) << endl;
	// Compute distances between other_* and fish_mean
	Log::i() << "Distances between other_* and fish_mean:" << endl;
	for(int i=0; i<4; i++) Log::i() << i << "-mean: " << MathUtils::covarianceDistance(c_other[i], fish_mean) << endl;
	// Compute distances between items and fish_mean
	Log::i() << "Distances between items and fish_mean:" << endl;
	for(int i=0; i<4; i++) Log::i() << i << "-mean: " << MathUtils::covarianceDistance(c_items[i], fish_mean) << endl;
	// Done
	Log::i() << "Done." << endl;*/
	

	/*try
	{
		theRNG() = RNG(time(NULL));
		Mat m1(10, 10, CV_64F);
		Mat m2(10, 10, CV_64F);
		theRNG().fill(m1, RNG::UNIFORM, 0, 100);
		theRNG().fill(m2, RNG::UNIFORM, 0, 100);
		Mat c[2];
		c[0] = MathUtils::computeCovarianceMatrix(m1);
		c[1] = MathUtils::computeCovarianceMatrix(m2);
		cout << c[0] << endl << endl;
		cout << c[1] << endl << endl;
		Mat c_mean = MathUtils::covarianceLieMean(c, 2, Mat(), false);
	}
	catch(MyException& e)
	{
		cout << e.what() << endl;
	}*/
	/*mat.at<Vec3b>(0,0) = Vec3b(0,20,40);
	mat.at<Vec3b>(0,1) = Vec3b(60,80,128);
	mat.at<Vec3b>(1,0) = Vec3b(150,170,190);
	mat.at<Vec3b>(1,1) = Vec3b(210,230,255);
	cvtColorYIQ(mat);*/
	//cout << "mat: " << endl << mat << endl << endl;
	//Mat new_mat = mat.clone();
	//new_mat.reshape(1, 4);
	//cout << "new_mat: " << endl << new_mat << endl << endl;
	/*string str_in = argv[1];
	vector<float> vec_out = convertFromString<vector<float> >(str_in);
	if(vec_out.size() > 0)
	{
		cout << vec_out[0];
		if(vec_out.size() > 1)
		{
			for(vector<float>::iterator it = vec_out.begin()+1; it != vec_out.end(); it++)
			{
				cout << " " << *it;
			}
		}
	}
	cout << endl;
	string str_out = convertToString<vector<float> >(vec_out);
	cout << str_out << endl;*/
	/*Mat img = Mat::zeros(50, 50, CV_8UC1);
	vector<Point> contour;
	contour.push_back(Point(10, 10));
	contour.push_back(Point(40, 15));
	contour.push_back(Point(40, 25));
	contour.push_back(Point(10, 15));
	contour = fixContour(contour);
	for(vector<Point>::iterator it = contour.begin(); it != contour.end(); it++)
	{
		img.at<uchar>(it->y, it->x) = 255;
	}
	imshow("img", img);
	waitKey(0);*/
	/*Mat img = Mat::zeros(50, 50, CV_8UC1);
	Point p1(atoi(argv[1]), atoi(argv[2]));
	Point p2(atoi(argv[3]), atoi(argv[4]));
	vector<Point> points = getPointsBetween(p1, p2);
	img.at<uchar>(p1.y, p1.x) = 255;
	img.at<uchar>(p2.y, p2.x) = 255;
	for(vector<Point>::iterator it = points.begin(); it != points.end(); it++)
	{
		img.at<uchar>(it->y, it->x) = 255;
	}
	imshow("img", img);
	waitKey(0);*/
	/*vector<list<int> > combinations = MathUtils::computeCombinations(atoi(argv[1]), atoi(argv[2]));
	for(vector<list<int> >::iterator c_it = combinations.begin(); c_it != combinations.end(); c_it++)
	{
		cout << "Combination: ";
		for(list<int>::iterator v_it = c_it->begin(); v_it != c_it->end(); v_it++)
		{
			cout << *v_it << " ";
		}
		cout << endl;
	}*/
	/*cout << sysconf(_SC_MONOTONIC_CLOCK) << endl;
	TimeInterval t;
	t.tic();
	cout << t.toc() << endl;*/
	/*Mat m(5,5,CV_8U);
	Mat_<uchar>& frame = (Mat_<uchar>&) m;
	frame << 0,0,0,128,255,128,128,0,128,128,255,255,128,0,0,128,128,0,255,0,0,0,128,0,128;
	//cout << frame << endl << endl;
	Mat glcm = computeCoOccurrenceMatrix(frame, Point(-1,-1), 3, false);
	cout << glcm << endl << endl;*/
	/*cout << "Model: " << MachineUtils::getCPUModel() << endl;
	cout << "Total memory: " << MachineUtils::getRAM() << endl;
	cout << "Process memory: " << MachineUtils::getRAMUsage() << endl;
	cout << "Cache size: " << MachineUtils::getCacheSize() << endl;*/
	/*for(int i=-10; i<10; i++)
	{
		cout << i << " % 5 = " << wrap(i, 5) << endl;
	}*/
	/*int s = 6;
	Point p1(1*s, 2*s);
	Point p2(1*s, 3*s);
	Point p3(2*s, 3*s);
	Point p4(2*s, 4*s);
	Point p5(3*s, 4*s);
	Point p6(3*s, 3*s);
	Point p7(4*s, 3*s);
	Point p8(4*s, 2*s);
	Point p9(3*s, 2*s);
	Point p10(3*s, 1*s);
	Point p11(2*s, 1*s);
	Point p12(2*s, 2*s);
	Point poly[] = {p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12};
	const Point* ptr = poly;
	const Point** ptrptr = &ptr;
	vector<Point> contour;
	contour.push_back(p1);
	contour.push_back(p2);
	contour.push_back(p3);
	contour.push_back(p4);
	contour.push_back(p5);
	contour.push_back(p6);
	contour.push_back(p7);
	contour.push_back(p8);
	contour.push_back(p9);
	contour.push_back(p10);
	contour.push_back(p11);
	contour.push_back(p12);
	Mat mat(50, 50, CV_8UC3);
	int num_points = 12;
	//fillPoly(mat, ptrptr, &num_points, 1, CV_RGB(255,255,255));
	//rectangle(mat, Point(10,10), Point(40,40), CV_RGB(255,255,255));
	Mat img = imread("test2.png");
	Mat mask;
	cvtColor(img, mask, CV_BGR2GRAY);
	cvb::CvBlobs blobs = computeBlobs(mask);
	cvb::CvBlob& blob = *(blobs.begin()->second);
	vector<Point> points = blobToPointVector(blob);
	//cout << boundaryComplexity(mask, points, 3, 3) << endl;
	//imshow("mat", mask);
	//waitKey(0);*/
}
