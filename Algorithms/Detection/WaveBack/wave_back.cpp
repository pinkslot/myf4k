// Template class for the implementation of a new detection algorithm

// ---------- CHANGE THIS ACCORDING TO THE NEW CLASS -------------------------------------------------------------------
#include "wave_back.h"

// ---------- CHANGE ALL CLASS NAMES ACCORDING TO THE NEW CLASS --------------------------------------------------------

namespace alg
{

	wave_back::wave_back()
	{
		// Setup parameters
		parameters.add<int>("n_frame_bkg", 2);
		// Variables
		read_frame = 0;
		mat_frame = NULL;
		bkg = NULL;
		mat_mask = NULL;
		//mat_norm = NULL;
		bkg_elem = NULL;
		mat_norm_size = 5;
	}
	void wave_back::add_to_frame_buffer(IplImage *frame)
	{
		int i = (read_frame % n_frame_bkg);
		for(int x=0;x<frame->width;x++)
			for(int y=0;y<frame->height;y++)
				cvSetReal3D(mat_frame,y,x,i,cvGetReal2D(frame,y,x));

	}

	void wave_back::add_to_mask_buffer(IplImage *frame)
	{
		int i = (read_frame % n_frame_bkg);
		for(int x=0;x<frame->width;x++)
			for(int y=0;y<frame->height;y++)
				cvSetReal3D(mat_mask,y,x,i,cvGetReal2D(frame,y,x));

	}

	// ---------- IMPLEMENT THIS METHOD! --------------------------------------------------------
	Mat wave_back::nextFrame(Context& context)
	{
		// Copy parameters to local variables
		n_frame_bkg = parameters.get<int>("n_frame_bkg");
		// Allocate memory
		IplImage frame_ipl = context.preproc_output;
		IplImage* frame = &frame_ipl;
		IplImage *copy = cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
		cvCvtColor(frame, copy, CV_RGB2GRAY);
		IplImage *out = cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
		IplImage *processed_frame=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
		//IplConvKernel* morphKernel = cvCreateStructuringElementEx(5, 5, 1, 1, CV_SHAPE_RECT, NULL);
		
		float alfa = 0.98;
		
		if(bkg == NULL)
		{
			int dims = 3;
			CvScalar v = cvScalar(255);
			int size[3] = {frame->height,frame->width,n_frame_bkg};
			int size_norm[3] = {frame->height,frame->width,mat_norm_size};
			bkg = cvCreateMatND(dims,size,CV_32F);
			mat_frame = cvCreateMatND(dims,size,CV_32F);
			mat_mask = cvCreateMatND(dims,size,CV_8U);
			mat_norm = cvCreateMatND(dims,size_norm,CV_32F);
			bkg_elem = cvCreateMat(n_frame_bkg,1,CV_32FC1);
			
			for(int x=0;x<frame->width;x++)
					for(int y=0;y<frame->height;y++)
					{
						for(int i=0;i<n_frame_bkg;i++)
							cvSetReal3D(mat_mask,y,x,i,255);
						for(int i=0;i<mat_norm_size;i++)
							cvSetReal3D(mat_norm,y,x,i,1);
					}

		}
		
		if(read_frame < n_frame_bkg)
		{
			add_to_frame_buffer(copy);
			read_frame++;
		}
		else
		{
			if(read_frame == n_frame_bkg)
			{
				float *pixel_bkg = new float[n_frame_bkg];
				for(int x=0;x<frame->width;x++)
					for(int y=0;y<frame->height;y++)
					{
						for(int i=0;i<n_frame_bkg;i++)
							pixel_bkg[i] = cvGetReal3D(mat_frame,y,x,i);

						CvMat tmp = cvMat(n_frame_bkg,1,CV_32FC1,pixel_bkg);
						CvMat dst_elem = cvMat(n_frame_bkg,1,CV_32FC1,pixel_bkg);
						// In "dst_elem" the DCT of [p1][p2] pixel intensity (BKG only)
						cvDCT(&tmp,&dst_elem,CV_DXT_FORWARD);
						//cvSmooth(&dst_elem,&dst_elem,CV_MEDIAN,2);
						for(int i=0;i<n_frame_bkg;i++)
							cvSetReal3D(bkg,y,x,i,cvGetReal2D(&dst_elem,i,0));						
					}

				delete []pixel_bkg;
			}

			add_to_frame_buffer(copy);
			read_frame++;
			
			float *pixel_frame = new float[n_frame_bkg];
			for(int x=0;x<frame->width;x++)
			{
				for(int y=0;y<frame->height;y++)
				{
					int i=(read_frame % n_frame_bkg);
					int j = 0;
					do
					{
						pixel_frame[j] = cvGetReal3D(mat_frame,y,x,i);
						i = (i+1) % n_frame_bkg;
						j++;
					} while(i!=(read_frame % n_frame_bkg));
					
					CvMat tmp = cvMat(n_frame_bkg,1,CV_32FC1,pixel_frame);
					CvMat dst_frg = cvMat(n_frame_bkg,1,CV_32FC1,pixel_frame);
					// In "dst_frg" the DCT of [p1][p2] pixel intensity (frame) 
					cvDCT(&tmp,&dst_frg,CV_DXT_FORWARD);
					//cvSmooth(&dst_frg,&dst_frg,CV_MEDIAN,2);
					
					for(int i=0;i<n_frame_bkg;i++) 
					{
						cvSetReal2D(bkg_elem,i,0,cvGetReal3D(bkg,y,x,i));
						cvSetReal3D(bkg,y,x,i,alfa*cvGetReal3D(bkg,y,x,i)+(1-alfa)*cvGetReal2D(&dst_frg,i,0));
						//cvSetReal2D(bkg_elem,i,0,cvGetReal3D(bkg,y,x,i));//(alfa*cvGetReal3D(bkg,y,x,i)+(1-alfa)*cvGetReal3D(mat_frame,y,x,i)));

					}	

					double norm = cvNorm(&dst_frg,bkg_elem,CV_L2);
					//double mul_norm = 1;
					//for(i=0;i<mat_norm_size;i++)
					//{
					//	double v = cvGetReal3D(mat_norm,y,x,i);
					//	mul_norm = mul_norm*v;
					//}
					
					if((norm>50))// &&(mul_norm<500000000))
						cvSetReal2D(out,y,x,255);
					else
						cvSetReal2D(out,y,x,0);

				}
			}
			delete []pixel_frame;		
			
			/*cvSmooth(out, processed_frame, CV_MEDIAN, 3, 3, 0, 0);
			cvErode(processed_frame, processed_frame, NULL, 1);
			cvDilate(processed_frame, processed_frame, NULL, 1);

			cvDilate(processed_frame, processed_frame, NULL, 1);
			cvErode(processed_frame, processed_frame, NULL, 1);
			cvAnd(out, processed_frame, processed_frame, NULL);*/
		
			#ifdef _DEBUG_
				cvShowImage("Output without Post-Processing", out);
				//cvShowImage("Output with Post-Processing", processed_frame);
			#endif
				
			//Images Multiplication for trail effect removal
			IplImage *mul_mask = cvCreateImage(cvGetSize(out),IPL_DEPTH_8U,1);
			add_to_mask_buffer(out);
			for(int x=0;x<out->width;x++)
			{
				for(int y=0;y<out->height;y++)
				{
					int val = 255;
					for(int i=0;i<n_frame_bkg;i++)
					{
						uchar t = cvGetReal3D(mat_mask,y,x,i);
						if(t != 255 || val != 255) val = 0;
					}
					cvSetReal2D(mul_mask,y,x,val);
				}
			}
			
			// cvLabel was here, on mul_mask
			Mat result(mul_mask, true);
			context.motion_output = result;
			 
			cvReleaseImage(&mul_mask);
		}
		
		cvReleaseImage(&copy);
		cvReleaseImage(&out);
		cvReleaseImage(&processed_frame);
		
		return context.motion_output;
	}	

	// ---------- IMPLEMENT THIS METHOD! --------------------------------------------------------
	wave_back::~wave_back()
	{
		cvReleaseMatND(&bkg);
		cvReleaseMatND(&mat_frame);
		cvReleaseMatND(&mat_mask);
		cvReleaseMat(&bkg_elem);
	}

}
