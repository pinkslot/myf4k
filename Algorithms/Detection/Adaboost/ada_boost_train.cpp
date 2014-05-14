#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <cv.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <image.h>
#include <detection.h>
#include <gaussian_mixture_model.h>
#include <apmm2.h>
#include <CodeBook.h>
#include <vibe2.h>
#include <IModel.h>
#include <wave_back.h>
#include <cvblob.h>
#include <log.h>
#include <string.h>
#include <blob_processing.h>
#include <pthread.h>
#include <tclap/CmdLine.h>

#include <string.h>

using namespace alg;
using namespace std;


typedef std::vector<float> ClassificationResults;
typedef std::vector<Detection*> CollectionClassifiers;

std::vector<std::vector<ClassificationResults > > weights;
std::vector<std::vector<std::vector<double> > > classResults;
std::vector<int> tp_count;
int v_width;
int v_height;
vector<IplImage*> thread_results;
struct ADATrainSet
{
    std::vector<IplImage*> outputs;
    IplImage* gt;
    IplImage* image;
};

struct WorkSet {
    Detection* classifier;
    IplImage* image;
    int classifier_order;
    bool debug;
};


std::vector<ADATrainSet> trainset;
CollectionClassifiers classifiers;
inline int getValue (int position, int algorithm, bool gt = false )
{

    int frame = position/(v_height*v_width);
    int pixel = position - frame*(v_height*v_width);
    int x = pixel%v_width;
    int y = pixel/v_width;
    double value = cvGetReal2D(gt==false?trainset[frame].outputs[algorithm]:trainset[frame].gt,y,x);
    return value>127?1:-1;
}





int getCoordinates(int image, int x, int y)
{
    return image*(v_height*v_width) + y*v_width + x;
}

std::vector<float> train(int epochs)
{
    int NUM_ALGORITHMS = classifiers.size();
    int NUM_TRAIN_SAMPLES = trainset.size();
    cout << "Images included in the training: " << NUM_TRAIN_SAMPLES <<endl;
    cout << "Algorithms included in the training: " <<endl;
    v_width = trainset[0].gt->width;
    v_height = trainset[0].gt->height;
    float init_weight  = (float)1/(float)NUM_ALGORITHMS;
    vector<int> votes;
    votes.resize(NUM_ALGORITHMS);
    for ( int c=0;c<NUM_ALGORITHMS;c++ )
    {
        cout << classifiers.at( c )->name() <<endl;
    }

    std::cout << "Algorithms trained successfully.\nResults obtained" << endl;


    ClassificationResults alpha;
    ClassificationResults D;
    int num_features = NUM_TRAIN_SAMPLES *v_height*v_width;
    D.resize (num_features);
    alpha.resize ( NUM_ALGORITHMS );
    for ( int c=0;c<NUM_ALGORITHMS;c++ )
    {
        alpha[c] = 0;
    }
    unsigned int num_current_classifier = 0;
    for ( unsigned int j=0; j < num_features; j++ )
    {
        D[j] = ( 1.0 ) /(float) num_features;

    }

    float min_classifier_weight;
    for ( unsigned int round=0; round < epochs; round++ )
    {
        ClassificationResults D_1 ( D );
        float min_error=num_features;
        unsigned int best_classifier = 0;
        float z = 0;
        cout << "Epoch " << round << endl;
        for ( num_current_classifier = 0;num_current_classifier < NUM_ALGORITHMS; num_current_classifier++ )
        {
            float error=0;

            for ( int j=0; j < num_features; j++ )
            {

                if ( getValue(j,num_current_classifier,true) !=getValue(j,num_current_classifier) )
                {

                    error += D[j];
                }
            }

            if ( error<min_error)
            {
                min_error = error;
                best_classifier = num_current_classifier;
                votes[best_classifier]++;
		cout << "Epoch "<< round << " best classifier: " << classifiers[best_classifier]->name() << " with min_error " << min_error<< endl;
            }
        }

        if ( min_error >= 0.5 )
        {
            cout << "Epoch stopped with min error " << min_error <<endl;
            break;
        }
        
        float alpha_sum =0;

        for (int i=0;i<alpha.size();i++)
        {
            alpha_sum+=alpha[i];
        }
	
	
        alpha[best_classifier] = log ( ( 1.0f - min_error ) / min_error ) /2;
	cout << "alpha[" << best_classifier << "] = " << alpha[best_classifier] << endl;
	if(round==0)
	{
	  min_classifier_weight = alpha[best_classifier];
	}
	else
	{
	  if(alpha[best_classifier]< min_classifier_weight) min_classifier_weight = alpha[best_classifier];
	}
        /*for (int i=0;i<alpha.size();i++)
        {
            alpha[i]/=alpha_sum;
        }*/

        for ( int j=0; j < num_features; j++ )
        {
            D_1[j] *= exp ( -alpha[best_classifier] * getValue ( j,best_classifier,true ) * getValue ( j,best_classifier ) );
            z+=D_1[j];
        }

        for ( unsigned int j=0; j < num_features; j++ )
        {
            D[j] = D_1[j]/z;
        }
    }
    bool ok = false;
    int norm_cnt = 1;
    cout << "Minimum classifier weight " << min_classifier_weight <<endl;
    for ( unsigned int j=0; j < alpha.size(); j++ )
    {
      
      if(alpha[j] < min_classifier_weight)alpha[j] = min_classifier_weight/2;
    }
    
    while (!ok)
    {
	cout << "Normalizing round " << norm_cnt << endl;
	cout << "Before: " << endl;
	for (int i=0;i<classifiers.size();i++)
        {

	  cout << classifiers[i]->name() <<" alpha "<< alpha[i] << endl;
   
	}
        ok = true;
        float tmp_sum = 0;
        for (int i=0;i<classifiers.size();i++)
        {
            if (alpha[i]>0.4)
            {
                alpha[i] = 0.39;
            }
            tmp_sum+=alpha[i];

        }
        for (int i=0;i<classifiers.size();i++)
        {
            alpha[i] /= tmp_sum;
        }
        for (int i=0;i<classifiers.size();i++)
        {
            if (alpha[i]>0.4)
            {
	      cout << "Classifier found bigger than 0.4 :" << classifiers[i]->name() << " con " << alpha[i]<<endl;
                ok=false;
                break;
            }
        }
        cout << "After: " << endl;
	for (int i=0;i<classifiers.size();i++)
        { 

	  cout << classifiers[i]->name() <<" alpha "<< alpha[i] << endl;
   
	}

    }


    ofstream file("ada_values");

    file << "\t\tmap<string,float> ada_values;" << endl;
    for (int i=0;i<classifiers.size();i++)
    {
        file <<"\t\tclassifier_names.push_back(\""<< classifiers[i]->name() <<"\");" << endl;
	file << "\t\tclassifier_weights.push_back(" << alpha[i] << ");" << endl;
    }

    for (int i=0;i<alpha.size();i++)
    {
        cout << classifiers[i]->name() << " got " <<votes[i] << " votes and alpha "<< alpha[i] << endl;
    }
    file.close();
    system("cat ada_bef ada_values ada_after > ada.h");
    return alpha;

}


std::vector<std::string>getdir (string dir )
{
    DIR *dp;
    std::vector<std::string> files;
    struct dirent *dirp;
    cout << "Searching directory " << dir << endl;
    if ((dp  = opendir(dir.c_str())) == NULL) {
        cout << "Error(" << errno << ") opening " << dir << endl;
        return files;
    }

    while ((dirp = readdir(dp)) != NULL) {
        if (strcmp(string(dirp->d_name).c_str(),".")==0 || strcmp(string(dirp->d_name).c_str(),"..")==0)continue;
        files.push_back(string(dirp->d_name));
    }

    closedir(dp);
    return files;
}


void *thread_next_frame( void *ptr )
{
    thread_results[((WorkSet*) ptr)->classifier_order]=((WorkSet*) ptr)->classifier->nextFrame(((WorkSet*) ptr)->image);
    //cout << ((WorkSet*) ptr)->classifier->name() << " returned." <<endl;
    if (((WorkSet*) ptr)->debug)cvShowImage(((WorkSet*) ptr)->classifier->name().c_str(),thread_results[((WorkSet*) ptr)->classifier_order]);
    delete ptr;
}

void initialize_algorithms(std::vector<string> algs_arg)
{
    for (int i=0;i<classifiers.size();i++)
    {

        delete classifiers.at(i);
    }
    classifiers.clear();
    if (algs_arg.size()==0)
    {
        APMM2 *apmm =  new alg::APMM2();
        classifiers.push_back (apmm );
        CodeBook *cb = new alg::CodeBook() ;
        classifiers.push_back (cb );
        IModel *im =  new alg::IModel();
        classifiers.push_back (im );
        GaussianMixtureModel *gmm = new alg::GaussianMixtureModel();
        classifiers.push_back (gmm );
        ViBE2 *vb = new alg::ViBE2();
        classifiers.push_back (vb );
        wave_back *wb  = new wave_back();
        classifiers.push_back (wb );
    }
    else for (int i=0;i<algs_arg.size();i++)

        {

            if (strcmp(algs_arg.at(i).c_str(),"apmm")==0)
            {
                APMM2 *apmm =  new alg::APMM2();
                classifiers.push_back (apmm );
                continue;
            }
            if (strcmp(algs_arg.at(i).c_str(),"cb")==0)
            {
                CodeBook *cb = new alg::CodeBook() ;
                classifiers.push_back (cb );
                continue;
            }
            if (strcmp(algs_arg.at(i).c_str(),"im")==0)
            {
                IModel *im =  new alg::IModel();
                classifiers.push_back (im );
                continue;
            }
            if (strcmp(algs_arg.at(i).c_str(),"agmm")==0)
            {
                GaussianMixtureModel *gmm = new alg::GaussianMixtureModel();
                classifiers.push_back (gmm );
                continue;
            }
            if (strcmp(algs_arg.at(i).c_str(),"vb")==0)
            {
                ViBE2 *vb = new alg::ViBE2();
                classifiers.push_back (vb );
                continue;
            }
            if (strcmp(algs_arg.at(i).c_str(),"wb")==0)
            {
                wave_back *wb  = new wave_back();
                classifiers.push_back (wb );
                continue;
            }


            cout << "Error: algorithm " << algs_arg.at(i) << " does not exist." <<endl;
            exit(-1);
        }

}

int main(int argc, char** argv)
{
    int rounds_arg,training_count_arg,ada_training_count_arg;
    bool debug_arg;
    vector<string> input_path_arg_v,input_gt_path_arg_v;
    vector<string> algs_arg;
    TCLAP::CmdLine cmd("Adaboost training program",' ',"1.0");
    string output_folder_arg;
    try {
        //"/home/isaak/Desktop/frames/%d.png"
        TCLAP::MultiArg<std::string> input_folder("i","input","Folder containing the input images",true,"string");
        TCLAP::MultiArg<std::string> input_gt_folder("g","gtinput","Folder containing the ground truth images. They should have the same names as the corresponding input images",true,"string");
        TCLAP::ValueArg<int> rounds("r","rounds","How many rounds should the adaboost training method run.",false,10,"int");
        TCLAP::ValueArg<int> training_count("t","traincount","How many images from the train set should be used to initialize the algorithms.",false,50,"int");
        TCLAP::ValueArg<int> ada_training_count("c","adatraincount","How many images from the train set should be used to initialize the algorithms.",true,50,"int");
        TCLAP::MultiArg<std::string> algs("a","algorithms","The algorithms that adaboost should be trained on. Valid values are: agmm apmm cb im vb wb",false,"string");
        TCLAP::ValueArg<std::string> output_file("o","output","The path for the output file",true,"./ada.h","string");
        TCLAP::SwitchArg debug("d","debug","Show debug information");

        cmd.add(input_folder);
        cmd.add(input_gt_folder);
        cmd.add(rounds);
        cmd.add(training_count);
        cmd.add(ada_training_count);
        cmd.add(algs);
        cmd.add(output_file);
        cmd.add(debug);
        cmd.parse( argc, argv );
        rounds_arg = rounds.getValue();
        input_path_arg_v = input_folder.getValue();
        input_gt_path_arg_v = input_gt_folder.getValue();
        ada_training_count_arg = ada_training_count.getValue();
        training_count_arg = training_count.getValue();
        debug_arg=debug.getValue();
        algs_arg = algs.getValue();
    }
    catch (TCLAP::ArgException &ex)
    {
        std::cerr << "error: " << ex.error() << " for arg " << ex.argId() << std::endl;
        return -1;
    }



    int class_size = 0;

    cout << "Starting reading input images" << endl;
    bool first = true;
    IplImage* no_detection;
    for (int tsets=0;tsets<input_path_arg_v.size();tsets++)
    {

        cout << "Test set " << tsets << endl;
        initialize_algorithms(algs_arg);    
	cout << "Algorithms initialized." << endl;
        CvBlobs blobs;
        class_size = classifiers.size();

        for (int i=1;i<training_count_arg;i++)
        {

            char tmp[100];
            sprintf(tmp,input_path_arg_v[tsets].c_str(),i);
            IplImage* train_image = cvLoadImage(tmp);
            if (debug_arg)cvShowImage("Input",train_image);
            for ( int c=0;c<class_size;c++ )
            {
	        cout << "Test set " << tsets << ", image " << i << ", algorithm " << classifiers.at(c)->name() << endl;
                IplImage* result = classifiers.at(c)->nextFrame(train_image);
                if (debug_arg)cvShowImage(classifiers.at(c)->name().c_str(),result);
                cvReleaseImage(&result);
                cvWaitKey(1);
            }
            cvReleaseImage(&train_image);
        }


        for (int i=training_count_arg;i<training_count_arg+ada_training_count_arg;i++)
        {

            char tmp[100];
            sprintf(tmp,input_path_arg_v[tsets].c_str(),i);
            if (debug_arg)cout << "Opening " << tmp << endl;
            IplImage* test = cvLoadImage(tmp);

            if (test==NULL)
            {

                cout << "Couldn't find image file " << tmp << ". \nContinuing." << endl;
                continue;

            }
            if (first)
            {
                no_detection= cvCreateImage(cvSize(test->width,test->height),IPL_DEPTH_8U,1);
                cvSetZero(no_detection);
                first = false;
            }
            ADATrainSet ts;
            sprintf(tmp,input_gt_path_arg_v[tsets].c_str(),i);

            IplImage* gt = (cvLoadImage(tmp,CV_LOAD_IMAGE_GRAYSCALE ));
            if (gt!=NULL)
            {
                ts.gt = gt;
            }
            else ts.gt = no_detection;

            for ( int c=0;c<classifiers.size();c++ )
            {
	        cout << "Test set " << tsets << ", image " << i << ", algorithm " << classifiers.at(c)->name() << endl;
                IplImage* result = classifiers.at( c )->nextFrame ( test);

                ts.outputs.push_back (result);


            }
            cvReleaseImage(&test);
            if (debug_arg)cout << "Trained with " << i << " images\r";
            if (debug_arg)cvShowImage("Ground truth",ts.gt);
            cvWaitKey(1);
            trainset.push_back(ts);
        }

    }
    std::vector<float> alpha = train(rounds_arg);
    cvReleaseImage(&no_detection);
    
    return 0;
}
