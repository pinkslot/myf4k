#ifndef DETECTION_APMM2_H
#define DETECTION_APMM2_H

#include "../detection.h"
#include <my_exception.h>
#include <vector>
#include <cv.h>
#include <highgui.h>
#include <cvblob.h>
#include <iostream>
#include <cstdio>

#define MINIMUM_PROBABILITY 0.00001

namespace alg
{

	struct PreDefinedPDFs
	{
		double pdf[256];
		double logValue;
		short l_min, l_max, centered_value;
	};


	class PDFLib
	{
		public:
		
		float pdflib[256][256];
		
		void readPDFs();
	};


	class PDF
	{
			
		void centerPDFTo(short graylevel);
		
		public:
		
		float weight;
		
		struct PreDefinedPDFs* pdf;

		PDF(short centered_value, float weight=1);

		inline bool intensityExists(short graylevel) { return (graylevel < pdf->l_max && graylevel > pdf->l_min) ? true : false; }
	};

	bool compare(const PDF* v1, const PDF* v2);

	class PDFVector
	{
		private:

		std::vector<PDF*> vec;
		short max_pdfs;
		float threshold, weight, min_prob;

		inline void addNew(short center_value) { vec.push_back(new PDF(center_value)); }

		public:

		PDFVector(short max_pdf_number, float thr, float wei, float min_pr)
		{
			this->max_pdfs = max_pdf_number;
			this->threshold = thr;
			this->weight = wei;
			this->min_prob = min_pr;
		}

		int isForeground(short idx)
		{
			float sum=0;
			int size = vec.size();
			for(int i=0; i<size; i++)
			{
				sum += vec.at(i)->weight;
				if(sum >= threshold && i < idx) return 1;
				else if(sum >= threshold && i >= idx) return 0;
			}
			return 0;
		}

		void normalize(float denominator)
		{
			/*for(int i=0;i<vec.size();i++)
			{
				vec.at(i)->weight/=denominator;
			}*/
		}

		int updatePDFWithMaxProbability(short gray_level);
		inline bool isEmpty() { return vec.size() == 0; }

	};


	class PDFManager
	{

		public:

		PDFManager(short gray_levels,short height,short width, short max_pdf_number, float, float, float);
		short height,width;
		
		std::vector<PDFVector*> PdfVector;
		
		short max_pdfs;
		float threshold, weight, min_prob;

		short gray_levels;

		inline PDFVector* getPixelPDF(short height,short width){ return PdfVector.at(height*this->width + width); }
	};


	class APMM2 : public Detection
	{
		public:
		
		PDFLib pdflib;
		bool first;
		int width,height,frameno;
		PDFManager* pdfManager;
		
		APMM2(void);
		~APMM2(void);
		
		void initialize();
		cv::Mat nextFrame(Context& context);
		float probslib[256][256];
		
		IplImage* output_img;

		inline string name() const { return "apmm"; }
                inline string description() const { return "Adaptive Poisson Mixture Model"; }
                inline string version() const { return "2.0"; }
                inline int executionTime() const { return 0; }
                inline int ram() const { return 0; }
	};

}

#endif
