#pragma once



#include <detection.h>
#include <cv.h>
#include <math.h>
#include <cvblob.h>
#include <deque>

#define RHO_B 20
#define RHO_F 10
#define K 0
#define ALPHA 0.0000001

struct BayesPoint{
	int x,y,r,g,b;
	bool is_in_foreground;
	bool is_in_background;
};

typedef struct BayesPoint* BayesFeature;
typedef struct std::deque<BayesFeature> Image;

class BayesianClassifier :
	public alg::Detection
{
public:
	void nextFrame(const IplImage* frame,cvb::CvBlobs& blobs);
	IplImage* nextFrame(IplImage* inimage);
	inline string name() const { return "bayes"; }
	inline string description() const { return "Bayesian detection combined to a Markov Random Field"; }
	inline string version() const { return "0.001"; }
	inline int executionTime() const { return 0; }
	inline int ram() const { return 0; }
	BayesianClassifier(void);
	~BayesianClassifier(void);
	
private:
	Image image;
	Image foreground;
	Image background;
	int hd,hr,frames_in_foreground,frames_in_background;
	int H_Discriminative;
	double gamma,n;
	double h_coefficient;
	double thr;
	bool first_frame;
	double alpha_gamma;
	vector<vector<double> > likelihood_ratio_map;
	float one_alpha;
	inline double get_likelihood_ratio(float p_b, float p_f){return -log(p_b/p_f);}
	inline int get_delta(float likelihood_ratio){return likelihood_ratio>0?0:255;}
	inline double gaussian_density(BayesFeature bf)
	{
	}
	inline BayesFeature get_vector_difference()
	{
		
	}
	inline double get_bkg_exp(BayesFeature x, BayesFeature y)
	{
	  BayesFeature bf = new BayesPoint();
	  bf->x = x->x - y->x;
	  bf->y = x->y - y->y;
	  bf->r = x->r - y->r;
	  bf->g = x->g - y->g;
	  bf->b = x->b - y->b;
	  double exp_calc = h_coefficient*exp(-0.5*(bf->x*0.04*bf->x + bf->y*bf->y*0.04 + bf->r*bf->r*0.0625 + bf->g*bf->g*0.0625 + bf->b*bf->b*0.0625)); 
	  delete bf; 
	  return exp_calc;
	  
	}
	inline double get_fore_exp(BayesFeature x, BayesFeature y)
	{
	  BayesFeature bf = new BayesPoint();
	  bf->x = x->x - y->x;
	  bf->y = x->y - y->y;
	  bf->r = x->r - y->r;
	  bf->g = x->g - y->g;
	  bf->b = x->b - y->b;
		
	  double exp_calc =alpha_gamma+ one_alpha* exp(-0.5*(bf->x*0.04*bf->x + bf->y*bf->y*0.04 + bf->r*bf->r*0.0625 + bf->g*bf->g*0.0625 + bf->b*bf->b*0.0625)); 
	  delete bf; 
	  return exp_calc;
	  
	}

};

