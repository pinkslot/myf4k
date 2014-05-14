#include "morph.h"
#include <cv.h>
#include <highgui.h>
#include <log.h>
// Debug includes
#include <iostream>

using namespace std;
using namespace cv;

namespace alg
{

	Morph::Morph()
	{
		// Setup parameters
		parameters.add<string>("op", "");
	}

	Morph::~Morph()
	{
	}

	Mat Morph::nextFrame(Context& context)
	{
		// Initialize output as current input
		context.postproc_output = context.motion_output.clone();
		// Split single operators
		vector<string> ops = splitString(parameters.get<string>("op"), '-');
		// Go through each operator
		for(vector<string>::iterator o_it = ops.begin(); o_it != ops.end(); o_it++)
		{
			// Get operator
			string& op = *o_it;
			// Split to get name and options
			vector<string> op_data = splitString(op, ':');
			if(op_data.size() != 2)
			{
				Log::warning() << "Invalid morphological operator description: " << op << endl;
				continue;
			}
			string op_name = op_data[0];
			string op_options = op_data[1];
			// In theory, op_options should be a list of key-value pairs. For now, we just assume it's the number of iteration for which we'll apply the operator
			int cycles = 0;
			try
			{
				cycles = convertFromString<int>(op_options);
			}
			catch(MyException& e)
			{
				Log::warning() << "Error reading cycle number for operator: " << op << ": " << e.what() << endl;
				continue;
			}
			// Apply operator
			if(op_name == "e")
			{
				erode(context.postproc_output, context.postproc_output, Mat(), Point(-1,-1), cycles);
			}
			else if(op_name == "d")
			{
				dilate(context.postproc_output, context.postproc_output, Mat(), Point(-1,-1), cycles);
			}
			else if(op_name == "m")
			{
				medianBlur(context.postproc_output, context.postproc_output, cycles);
			}
			else
			{
				Log::warning() << "Unknown morphological operator: " << op_name << endl;
				continue;
			}
		}
		// Return result
		return context.postproc_output;
	}
		
}
