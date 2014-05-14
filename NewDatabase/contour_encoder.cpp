#include "contour_encoder.h"
#include <my_exception.h>

using namespace std;
using namespace cv;

namespace db
{

void ContourEncoder::decodePointsFromBytes(std::string ccstr,Point first, vector<Point>& cc)
{

    Point refPoint = first;
	cc.push_back(first);
	for (unsigned int i = 0; i < ccstr.length(); i += 3)
    {
        Point p;
				
		std::bitset<3> frag(std::string(ccstr.substr(i, 3)));
		switch (frag.to_ulong())
        {
            case 0:
                p.x = refPoint.x + 1;
                p.y = refPoint.y;
                break;
            case 1:
                p.x = refPoint.x + 1;
                p.y = refPoint.y + 1;
                break;
            case 2:
                p.x = refPoint.x ;
                p.y = refPoint.y + 1;
                break;
            case 3:
                p.x = refPoint.x - 1;
                p.y = refPoint.y + 1;
                break;
            case 4:
                p.x = refPoint.x - 1;
                p.y = refPoint.y;
                break;
            case 5:
                p.x = refPoint.x - 1;
                p.y = refPoint.y - 1;
                break;
            case 6:
                p.x = refPoint.x;
                p.y = refPoint.y -1;
                break;
            case 7:
                p.x = refPoint.x +1;
                p.y = refPoint.y -1;
                break;
        }
		cc.push_back(p);
        refPoint = p;
        if (p.x == first.x && p.y == first.y) break;
               
             
    }
   
}

 string ContourEncoder::encodePointsToBytes(const vector<Point>& pointList)
{
    string directions ="";
           
    string s = "";
	for (unsigned int i = 1; i < pointList.size(); i++)
    {
        Point p = pointList[i-1];
        Point ptmp = pointList[i];
        if (p.x == ptmp.x && p.y == ptmp.y) { continue; }//return directions;}
        else if (p.x == ptmp.x - 1 && p.y == ptmp.y) directions+=("000");
        else if (p.x == ptmp.x - 1 && p.y == ptmp.y - 1) directions += ("001");
        else if (p.x == ptmp.x     && p.y == ptmp.y - 1) directions += ("010");
        else if (p.x == ptmp.x + 1 && p.y == ptmp.y - 1) directions += ("011");
        else if (p.x == ptmp.x + 1 && p.y == ptmp.y) directions += ("100");
        else if (p.x == ptmp.x + 1 && p.y == ptmp.y + 1) directions += ("101");
        else if (p.x == ptmp.x     && p.y == ptmp.y + 1) directions += ("110");
        else if (p.x == ptmp.x - 1 && p.y == ptmp.y + 1) directions += ("111");
	else throw MyException("ContourEncoder: invalid chain-code.");

                
    }
    return directions;
           
}

ContourEncoder::EncodingResult ContourEncoder::chardec2bin(unsigned char n,unsigned int padding){
    char result[(sizeof(unsigned)*8)+1];
    unsigned index=sizeof(unsigned)*8;
    result[index]='\0';
    do result[--index]='0'+(n&1);
    while (n>>=1);
	string tmp = (std::string(result+index));
	EncodingResult encoding_result;;
    if(padding>0)
	{
		stringstream ss;

		for(unsigned int i=tmp.length();i<padding;i++)
		{
			ss << '0';
		}

		ss << tmp;
		tmp=ss.str();
		
	}
	if(tmp.size()>padding)
	{
		encoding_result.result = BIT_OVERFLOW;
		throw MyException("ContourEncoder: overflow error.");
	}
	else encoding_result.result = OK;
	encoding_result.resultString = tmp;
	return encoding_result;
}

ContourEncoder::EncodingResult ContourEncoder::intdec2bin(unsigned int n,unsigned int padding){
    char result[(sizeof(unsigned)*8)+1];
    unsigned index=sizeof(unsigned)*8;
    result[index]='\0';
    do result[--index]='0'+(n&1);
    while (n>>=1);
	string tmp = (std::string(result+index));
	EncodingResult encoding_result;;
    if(padding>0)
	{
		stringstream ss;

		for(unsigned int i=tmp.length();i<padding;i++)
		{
			ss << '0';
		}

		ss << tmp;
		tmp=ss.str();
		
	}
	if(tmp.size()>padding)
	{
		encoding_result.result = BIT_OVERFLOW;
		throw MyException("ContourEncoder: overflow error.");
	}
	else encoding_result.result = OK;
	encoding_result.resultString = tmp;
	return encoding_result;
}

std::string ContourEncoder::encode(const vector<Point>& bounding_box, const vector<Point>& contour)
{
	string encoded;
            
    vector<Point> origPointList;
    origPointList =contour;
    // Fix contour, in order to have the top-most point first
    // Iterator to top-most point
    vector<Point>::iterator top_it = origPointList.begin();
    vector<Point>::iterator curr = top_it;
    for(curr++; curr != origPointList.end(); curr++)
    {
	    if(curr->y < top_it->y || (curr->y == top_it->y && curr->x < top_it->x))
	    {
		    top_it = curr;
	    }
    }
    // Create new vector
    vector<Point> pointList;
    for(vector<Point>::iterator it = top_it; it != origPointList.end(); it++)
    {
	    pointList.push_back(*it);
    }
    for(vector<Point>::iterator it = origPointList.begin(); it != top_it; it++)
    {
	    pointList.push_back(*it);
    }
    // Fix contour
    pointList = fixContour(pointList);
    string asd = "";

    //for(vector<Point>::iterator it = pointList.begin(); it != pointList.end(); it++) cout << it->x << " " << it->y << ",";
    //cout << endl;

    vector<Point> bbPoints = bounding_box;
    Point p = bbPoints.at(0);
	
	
    EncodingResult ex = intdec2bin(p.x,11);//.PadLeft(11,'0');
    EncodingResult ey = intdec2bin(p.y,11);//.PadLeft(11, '0');
    EncodingResult eh = intdec2bin(bbPoints[2].y - p.y,10);//.PadLeft(10, '0');
    EncodingResult ew = intdec2bin(bbPoints[2].x - p.x,10);//.PadLeft(10, '0');
    EncodingResult efp = intdec2bin(pointList[0].x,11);//.PadLeft(11, '0');
	string x;
    x = ex.resultString + ey.resultString + eh.resultString + ew.resultString + efp.resultString;
    string encoded_dirs = encodePointsToBytes(pointList);
            
    int padding = encoded_dirs.length()%8;
	
    if(padding==0)x+="000";
	else
	{
		
			x+=intdec2bin(8-padding,3).resultString;
	}
	//std::cout << x << ":" << encoded_dirs <<std::endl;
	encoded_dirs = x+encoded_dirs;
	int exact_bits = encoded_dirs.length() - padding;
	int cnt = exact_bits;
	if(padding>0)cnt = exact_bits-1;
	for (int i = 0; i <cnt; i += 8)
    {
        //unsigned char b;
        string bytestr = "";
      
        for (int j = 0; j < 8; j++)
        {
            bytestr += encoded_dirs[i + j];
        }
		//std::reverse(bytestr.begin(), bytestr.end());
		std::bitset<8> bitx (string(bytestr.c_str()));
	   
		encoded+=((unsigned char)bitx.to_ulong());
    }


	
	if(padding>0)
	{
		
		string bytestr = "";

		for(unsigned int k= exact_bits;k< encoded_dirs.length(); k++)
		{
			 bytestr += encoded_dirs[k];
		}
		for(int k=0;k<8 - padding;k++)
		{
			bytestr+= '0';
		}
		//std::reverse(bytestr.begin(), bytestr.end());
		std::bitset<8> bitx (string(bytestr.c_str()));
	    encoded.push_back((unsigned char)bitx.to_ulong());
	}

	
	return encoded;
}

std::string ContourEncoder::encode(string bb, string cc)
{
    string encoded;
            
    vector<Point> pointList;
    pointList = GetPointsFromString(cc);
    string asd = "";

	for (unsigned int k=0;k<pointList.size();k++)
    {
		ostringstream str;
		Point pt = pointList.at(k);
		str << pt.x <<  "," << pt.y << ";";
    }
    vector<Point> bbPoints = GetPointsFromString(bb);
    Point p = bbPoints.at(0);
	ostringstream cvt;
	
    EncodingResult ex = intdec2bin(p.x,11);//.PadLeft(11,'0');
    EncodingResult ey = intdec2bin(p.y,11);//.PadLeft(11, '0');
    EncodingResult eh = intdec2bin(bbPoints[2].y - p.y,10);//.PadLeft(10, '0');
    EncodingResult ew = intdec2bin(bbPoints[2].x - p.x,10);//.PadLeft(10, '0');
    EncodingResult efp = intdec2bin(pointList[0].x,11);//.PadLeft(11, '0');
	string x;
    x = ex.resultString + ey.resultString + eh.resultString + ew.resultString + efp.resultString;
    string encoded_dirs = encodePointsToBytes(pointList);
            
    int padding = encoded_dirs.length()%8;
	//cout << encode(bbPoints,pointList) << endl;
    if(padding==0)x+="000";
	else
	{
		
			x+=intdec2bin(8-padding,3).resultString;
	}
	//std::cout << x << ":" << encoded_dirs <<std::endl;
	encoded_dirs = x+encoded_dirs;
	int exact_bits = encoded_dirs.length() - padding;
	int cnt = exact_bits;
	if(padding>0)cnt = exact_bits-1;
	for (int i = 0; i <cnt; i += 8)
    {
        //unsigned char b;
        string bytestr = "";
      
        for (int j = 0; j < 8; j++)
        {
            bytestr += encoded_dirs[i + j];
        }
		//std::reverse(bytestr.begin(), bytestr.end());
		std::bitset<8> bitx (string(bytestr.c_str()));
	   
		encoded+=((unsigned char)bitx.to_ulong());
    }

	
	
	if(padding>0)
	{
		
		string bytestr = "";

		for(unsigned int k= exact_bits;k< encoded_dirs.length(); k++)
		{
			 bytestr += encoded_dirs[k];
		}
		for(int k=0;k<8 - padding;k++)
		{
			bytestr+= '0';
		}
		//std::reverse(bytestr.begin(), bytestr.end());
		std::bitset<8> bitx (string(bytestr.c_str()));
	    encoded.push_back((unsigned char)bitx.to_ulong());
	}

	
	return encoded;
            
}

  vector<Point> ContourEncoder::GetPointsFromString(string s)
{
        vector<Point> contourPoints;// = new v<Point>();    
        string tmp = string(s);
		string cc = tmp.erase(0,9);
		cc = cc.erase(cc.length()-2,2);
		vector<string> pointsStr = splitstr(cc,',');
	
		for(unsigned int i=0;i<pointsStr.size();i++)
		{
			vector<string> coordinates  = splitstr(pointsStr.at(i),' ');
			Point p;
			istringstream ( coordinates.at(0) ) >> p.x;
			istringstream ( coordinates.at(1) ) >> p.y;
			contourPoints.push_back(p);
		}
       
        return contourPoints;
         
           
}

 std::vector<std::string> &ContourEncoder::split(const std::string &s, char delim, std::vector<std::string> &elems) {
    stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


 vector<std::string> ContourEncoder::splitstr(std::string &s, char delim) {
    std::vector<std::string> elems;
    return split(s, delim, elems);
}

 void ContourEncoder::decode(vector<Point>& bb, vector<Point>& cc,string data)
 {
	string str = data.substr(0,7);
	string bbstr="",ccstr="";
	for(int i=0;i<7;i++)
	{
		 bbstr+= chardec2bin(str.at(i),8).resultString;

	}

	str = data.substr(7,data.length()-7);
	for(unsigned int i=0;i<str.length();i++)
	{
		 ccstr+= chardec2bin(str.at(i),8).resultString;

	}
	
	Point firstBBPoint,firstCCPoint;
	char * ptr;
	firstBBPoint.x = strtol(bbstr.substr(0,11).c_str(),&ptr,2);
	firstBBPoint.y = strtol(bbstr.substr(11,11).c_str(),&ptr,2);
	bb.push_back(firstBBPoint);
	int h = strtol(bbstr.substr(22,10).c_str(),&ptr,2);
    int w = strtol(bbstr.substr(32,10).c_str(),&ptr,2);
	int padding = strtol(bbstr.substr(53,3).c_str(),&ptr,2);

	if(padding>0)
	{
		for(int i=0;i<padding;i++)
		{
			ccstr = ccstr.substr(0, ccstr.size()-1);
			//ccstr.pop_back();
		}
	}
	firstCCPoint.x = strtol(bbstr.substr(42,11).c_str(),&ptr,2);
	firstCCPoint.y = firstBBPoint.y;
	Point ptmp;
	ptmp.x = firstBBPoint.x + w;
	ptmp.y = firstBBPoint.y;
	bb.push_back(ptmp);
	
	ptmp.x = firstBBPoint.x + w;
	ptmp.y = firstBBPoint.y + h;
	bb.push_back(ptmp);

	ptmp.x = firstBBPoint.x;
	ptmp.y = firstBBPoint.y+h;
	bb.push_back(ptmp);	
	bb.push_back(firstBBPoint);
	
	decodePointsFromBytes(ccstr,firstCCPoint,cc);
	

	//cout << bbstr << ":" << ccstr <<endl;
 }

vector<Point> ContourEncoder::decode(string bb_cc_str)
{
	vector<Point> bb, cc;
	decode(bb, cc, bb_cc_str);
	return cc;
}

}
