#include <cv.h>
#include <highgui.h>
#include <lemon/list_graph.h>
#include <lemon/preflow.h>
#include "postprocessing.h"

namespace alg
{

class MRF : public PostProcessing
{
private:
public:

  MRF() {}
  ~MRF() {}
  static IplImage* openCVToListGraph(IplImage* img);
 
  inline string name() const { return "blob_mrf"; }
  inline string description() const { return "Markov Random Fields Smoothing"; }
  inline string version() const { return "1.0"; }
  inline int executionTime() const { return 0; }
  inline int ram() const { return 0; }
 
  cv::Mat nextFrame(Context& context);
};



template <typename T>
class DynamicArray
{
public:
    DynamicArray(){};

    DynamicArray(int rows, int cols): dArray(rows, vector<T>(cols)){}

    vector<T> & operator[](int i) 
    { 
      return dArray[i];
    }
    const vector<T> & operator[] (int i) const 
    { 
      return dArray[i];
    }
    void resize(int rows, int cols)//resize the two dimentional array .


    {
        dArray.resize(rows);
        for(int i = 0;i < rows;++i) dArray[i].resize(cols);
    }
private:
    vector<vector<T> > dArray;  
};


struct NodeCoordinates{
  int x,y;
};
}
