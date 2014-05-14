#include "mrf.h"

using namespace cv;
using namespace std;

namespace alg{

cv::Mat MRF::nextFrame(Context& context)
{
  IplImage tmp = context.motion_output;
  IplImage* tmp_out =  openCVToListGraph(&tmp);
  cv::Mat tmp_out_mat(tmp_out,true);
  cvReleaseImage(&tmp_out);
  context.postproc_output = tmp_out_mat;
  return tmp_out_mat;
}

IplImage* MRF::openCVToListGraph(IplImage* img)
{
  map<int,NodeCoordinates> nodes_coordinates;
  
#ifdef DEBUG
  int cnt=0,cnt_t=0,cnt_f=0,p_cnt_b=0,p_cnt_f=0;
#endif
  
  DynamicArray<lemon::ListDigraph::Node> arr(img->width,img->height);
  lemon::ListDigraph lDiGraph;
  lemon::ListDigraph::ArcMap<double> map_bw(lDiGraph);
  lemon::ListDigraph::Node source = lDiGraph.addNode();
  lemon::ListDigraph::Node sink = lDiGraph.addNode();

 
  for(int j=0;j<img->width;j++)
  {
    
    arr[j][0] = lDiGraph.addNode();
    arr[j][img->height-1] = lDiGraph.addNode();
  }
  
  for(int j=1;j<img->height-1;j++)
  {
    arr[0][j] = lDiGraph.addNode();
    arr[img->width-1][j] = lDiGraph.addNode();
  }
  
  
  for(int i=1;i<img->width-1;i++)
  {

    for(int j=1;j<img->height-1;j++)
    {
      
      {
	lemon::ListDigraph::Node node = lDiGraph.addNode();
	
	arr[i][j] = node;
	struct NodeCoordinates nc;
	
	nc.x = i;
	nc.y = j;
	nodes_coordinates.insert(pair<int,NodeCoordinates>(lDiGraph.id(node),nc));
	double val = cvGetReal2D(img,j,i);
	if(val==0)
	{
	  map_bw[lDiGraph.addArc(node,sink)]=1;
#ifdef DEBUG
	  p_cnt_b++;
#endif
	}
	else if(val==255)
	{
	  map_bw[lDiGraph.addArc(source,node)]=1;
#ifdef DEBUG
	  p_cnt_f++;
#endif
	}
	
	map_bw[lDiGraph.addArc(node, arr[i-1][j])]=1;
	
	map_bw[lDiGraph.addArc(node, arr[i][j-1])]=1;
	
	map_bw[lDiGraph.addArc(arr[i-1][j],node)]=1;
	
	map_bw[lDiGraph.addArc( arr[i][j-1],node)]=1;
	
      }
    }
    
  }
  
  
  
  lemon::Preflow<lemon::ListDigraph, lemon::ListDigraph::ArcMap< double > > conn_graph(lDiGraph,map_bw,source,sink);
  
  IplImage* result = cvCreateImage(cvSize(img->width,img->height),IPL_DEPTH_8U,1);
  conn_graph.run();
  for (lemon::ListDigraph::NodeIt n(lDiGraph); n != lemon::INVALID; ++n)
  {
    if(nodes_coordinates[lDiGraph.id(n)].x==0 && nodes_coordinates[lDiGraph.id(n)].y==0) continue;
   
    if(conn_graph.minCut(n))
    {
     
      cvSetReal2D(result,nodes_coordinates[lDiGraph.id(n)].y,nodes_coordinates[lDiGraph.id(n)].x,255);
#ifdef DEBUG
      cnt_t++;
#endif
    }
    else
    {
      cvSetReal2D(result,nodes_coordinates[lDiGraph.id(n)].y,nodes_coordinates[lDiGraph.id(n)].x,0);
#ifdef DEBUG
      cnt_f++;

    }
    
    cnt++;
#else
  }
#endif
  }
  
#ifdef DEBUG  

  cvShowImage("Result",result);
  std::cout << "Number of nodes: " << cnt << std::endl;
  std::cout << "Number of true nodes: " << cnt_t << std::endl;
  std::cout << "Number of false nodes: " << cnt_f << std::endl;
  std::cout << "Number of back pixels: " << p_cnt_b << std::endl;
  std::cout << "Number of white pixels: " << p_cnt_f << std::endl;
  cout << "Flow value:" << conn_graph.flowValue() << endl;
#endif
  
  return result;
}

}
