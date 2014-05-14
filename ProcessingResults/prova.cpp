#include "keyframes.h"

using namespace std;

int main(int argc, char** argv)
{
	Log::setDebugLogStream(cout);
	KeyFrames kf;
	kf.add(10, 20);
	kf.add(30, 40, "algae");
	kf.add(50, 60, "fish");
	kf.add(50, 70);
	kf.setLabel(argc > 1 ? argv[1] : "");
	cout << kf << endl;
	list<string> labels = kf.listLabels();
	cout << "Labels: ";
	for(list<string>::iterator it = labels.begin(); it != labels.end(); it++)
		cout << *it << " ";
	cout << endl;
	cout << "5: " << kf.check(5) << endl;
	cout << "15: " << kf.check(15) << endl;
	cout << "25: " << kf.check(25) << endl;
	cout << "35: " << kf.check(35) << endl;
	cout << "70: " << kf.check(70) << endl;
	cout << "36: " << kf.check(36) << endl;
	cout << "37: " << kf.check(37) << endl;
	cout << "55: " << kf.check(55) << endl;
	cout << "75: " << kf.check(75) << endl;
}
