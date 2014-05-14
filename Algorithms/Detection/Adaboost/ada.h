#include <detection.h>
#include <vector>
#include <tclap/CmdLine.h>
#include <gaussian_mixture_model.h>
#include <apmm2.h>
#include <CodeBook.h>
#include <vibe2.h>
#include <IModel.h>
#include <wave_back.h>
#include <cvblob.h>

//using namespace alg


class AdaBoost:public alg::Detection
{
private:
    std::vector<alg::Detection*> classifiers;
    std::vector<string> classifier_names;
    std::vector<float> classifier_weights;
   
    inline string name() const {
        return "ada";
    }
    inline string description() const {
        return "Adaboost";
    }
    inline string version() const {
        return "1.0";
    }
    inline int executionTime() const {
        return 0;
    }
    inline int ram() const {
        return 0;
    }
    
    
    cv::Mat nextFrame(Context& context);
public:
    AdaBoost()
    {		map<string,float> ada_values;
	        parameters.add<string>("dummy", "1");
		classifier_names.push_back("apmm");
		classifier_weights.push_back(0.14);
		classifier_names.push_back("cb");
		classifier_weights.push_back(0.14);
		classifier_names.push_back("im");
		classifier_weights.push_back(0.14);
		classifier_names.push_back("agmm");
		classifier_weights.push_back(0.3);
		classifier_names.push_back("vibe");
		classifier_weights.push_back(0.14);
		classifier_names.push_back("wb");
		classifier_weights.push_back(0.14);


        for (unsigned int i=0;i<classifier_names.size();i++ )
        {
            

                if (strcmp(classifier_names[i].c_str(),"apmm")==0)
                {
                    alg::APMM2 *apmm =  new alg::APMM2();
                    classifiers.push_back (apmm );
                    continue;
                }
                if (strcmp(classifier_names[i].c_str(),"cb")==0)
                {
                    alg::CodeBook *cb = new alg::CodeBook() ;
                    classifiers.push_back (cb );
                    continue;
                }
                if (strcmp(classifier_names[i].c_str(),"im")==0)
                {
                    alg::IModel *im =  new alg::IModel();
                    classifiers.push_back (im );
                    continue;
                }
                if (strcmp(classifier_names[i].c_str(),"agmm")==0)
                {
                    alg::GaussianMixtureModel *gmm = new alg::GaussianMixtureModel();
                    classifiers.push_back (gmm );
                    continue;
                }
                if (strcmp(classifier_names[i].c_str(),"vb")==0)
                {
                    alg::ViBE2 *vb = new alg::ViBE2();
                    classifiers.push_back (vb );
                    continue;
                }
                if (strcmp(classifier_names[i].c_str(),"wb")==0)
                {
                    alg::wave_back *wb  = new alg::wave_back();
                    classifiers.push_back (wb );
                    continue;
                }
             
            
        }

    }
};
