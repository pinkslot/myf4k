#include <cv.h>
#include <highgui.h>
#include <cmath>
#include <iostream>
#include "classifier.h"

using namespace std;
using namespace cv;
using namespace alg;


int main()
{
	Classifier c;
	try
	{
		c.trainModel("/mnt/fst/train/fish/");
	}
	catch(MyException &e)
	{
		cout << "Error filling classifier." << e.what() << endl;
		exit(1);
	}
	cout << c.predict(imread("good_msk.bmp"));	

}
