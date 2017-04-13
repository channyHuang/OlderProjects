#pragma once

#include <cxcore.h>
#include <cv.h>


/*
Converts an image to 32-bit grayscale

@param img a 3-channel 8-bit color (BGR) or 8-bit gray image

@return Returns a 32-bit grayscale image
*/
static IplImage* convert_to_gray32( IplImage* img )
{
	//IplImage* gray8, * gray32;
	IplImage * gray32;
	gray32 = cvCreateImage( cvGetSize(img), IPL_DEPTH_32F, 1 );

	int w_step = img->nChannels*img->depth/8;

	for(int y=0; y<img->height; y++)
	{
		int index = img->widthStep*y;
		for(int x=0; x<img->width; x++)
		{
			uchar* imgPtr=(uchar*)(img->imageData + index + x* w_step) ;
		    int r = *imgPtr; 
			int g = *(imgPtr+1);
			int b = *(imgPtr+2);

			float i_gray = (b*11+g*16+r*5)/32;

			float* gray = (float*)(gray32->imageData+gray32->widthStep*y+x * gray32->depth/8) ;

			*gray = i_gray;
		}
		
	}

	/*gray32 = cvCreateImage( cvGetSize(img), IPL_DEPTH_32F, 1 );
	if( img->nChannels == 1 )
		gray8 = (IplImage*)cvClone( img );
	else
	{
		gray8 = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 1 );
		cvCvtColor( img, gray8, CV_BGR2GRAY );
	}
	cvConvertScale( gray8, gray32, 1.0 / 255.0, 0 );
	cvReleaseImage( &gray8 );*/
	return gray32;
}

/*
Downsamples an image to a quarter of its size (half in each dimension)
using nearest-neighbor interpolation

@param img an image

@return Returns an image whose dimensions are half those of img
*/
static IplImage* downsample( IplImage* img )
{
	IplImage* smaller = cvCreateImage( cvSize(img->width / 2, img->height / 2),
		img->depth, img->nChannels );
	//cvResize( img, smaller, CV_INTER_NN );

	return smaller;
}

/*
Builds Gaussian scale space pyramid from an image

@param base base image of the pyramid
@param octvs number of octaves of scale space
@param sigma amount of Gaussian smoothing per octave

@return Returns a Gaussian scale space pyramid as an octvs  array
*/
static IplImage** build_gauss_pyr( IplImage* base, int levels )
{
	IplImage** gauss_pyr;

	

	gauss_pyr = (IplImage**)calloc( levels, sizeof( IplImage* ) );

	for(int o = 0; o < levels; o++ )
	{
		if( o == 0 )
			gauss_pyr[o] = cvCloneImage(base);
		/* base of new octvave is halved image from end of previous octave */
		/* blur the current octave's last image to create the next one */
		else
		{
			//gauss_pyr[o] = downsample( gauss_pyr[o-1]);
			//cvSmooth( gauss_pyr[o], gauss_pyr[o], CV_GAUSSIAN, 3, 3, sigma, sigma );

			 gauss_pyr[o] = downsample( gauss_pyr[o-1]);
			
			 cvPyrDown(gauss_pyr[o-1], gauss_pyr[o]); 
		}
	}
	return gauss_pyr;
}

static IplImage* stack_imgs( IplImage* img1, IplImage* img2 )
{
	/*IplImage* stacked = cvCreateImage( cvSize(  img1->width + img2->width ,MAX(img1->height, img2->height)),
										IPL_DEPTH_8U, 3 );*/
	IplImage* stacked = cvCreateImage( cvSize(  MAX(img1->width ,img2->width) ,img1->height+img2->height),
		img1->depth, img1->nChannels );

	cvZero( stacked );
	cvSetImageROI( stacked, cvRect( 0, 0, img1->width, img1->height ) );
	cvAdd( img1, stacked, stacked, NULL );
	cvSetImageROI( stacked, cvRect(0, img1->height, img2->width, img2->height) );
	//cvSetImageROI( stacked, cvRect( img1->width, 0, img2->width, img2->height) );
	cvAdd( img2, stacked, stacked, NULL );
	cvResetImageROI( stacked );

	return stacked;
}

static IplImage* downsample( IplImage* img, int level )
{
	if(level==0)
		return img;

	 IplImage* t_img0 = cvCloneImage(img);

	

	 IplImage* t_img1;
	for(int i=0;i<level; i++)
	{
		t_img1 = downsample(t_img0);

		cvResize( t_img0, t_img1, CV_INTER_NN );
		cvReleaseImage( &t_img0 );

		t_img0 = t_img1;
	}

	return t_img1;
}
/*************** RGB NCC ************************************/

//RGB NCC
static void Normalize(std::vector<float>& tex)
{
	const int size = (int)tex.size();
	const int size3 = size / 3;
	float ave[3];

	ave[0] = 0;
	ave[1] = 0;
	ave[2] = 0;

	float* texp = &tex[0] - 1;
	for (int i = 0; i < size3; ++i) {
		ave[0] += *(++texp);
		ave[1] += *(++texp);
		ave[2] += *(++texp);
	}

	ave[0] /= size3;
	ave[1] /= size3;
	ave[2] /= size3;

	float ave2 = 0.0;
	texp = &tex[0] - 1;
	for (int i = 0; i < size3; ++i) {
		const float f0 = ave[0] - *(++texp);
		const float f1 = ave[1] - *(++texp);
		const float f2 = ave[2] - *(++texp);

		ave2 += f0 * f0 + f1 * f1 + f2 * f2;
	}

	ave2 = sqrt(ave2 / size);

	if (ave2 == 0.0f)
		ave2 = 1.0f;

	texp = &tex[0] - 1;
	for (int i = 0; i < size3; ++i) {
		*(++texp) -= ave[0];    *texp /= ave2;
		*(++texp) -= ave[1];    *texp /= ave2;
		*(++texp) -= ave[2];    *texp /= ave2;
	}
}

static double RGB_NCC(std::vector<float>& v0, std::vector<float>& v1)
{
	double NCC_Value = 0;
	const int size = (int)v0.size();
	const int size3 = size / 3;

	for (int i = 0; i < size3; ++i) 
	{
		for(int k=0; k<3; k++)
		{

			NCC_Value+= v0[3*i+k]*v1[3*i+k];
		}

	}

	NCC_Value = NCC_Value/size;

	return NCC_Value;
}
/******************* end RGB NCC ***********************************/


