#pragma once

static const double pi = 3.14159265358979323846;



#define  SwapImage(type,a,b)	\
{						\
	type * temp = a;\
	a = b;				\
	b = temp;			\
}

//void swapImage(IplImage * psrc,IplImage * pdest)
//{
//	IplImage * temp = psrc;
//	psrc = pdest;
//	pdest = temp;
//}



inline static bool ClearImage(IplImage * pImage, int index)
{
	//ImageFormat ^ imageFormat = pImage->RawFormat->Bmp;

	BYTE * pSrc = (BYTE*)pImage->imageData;

	memset(pImage->imageData,index,pImage->widthStep* pImage->height);

	return true;
}


inline static void allocateOnDemand( IplImage **img, int w, int h, int depth, int channels )
{
	CvSize size = cvSize( w, h );

	if ( *img != NULL )	
	{
		ClearImage(*img,0);
		return;
	}

	*img = cvCreateImage( size, depth, channels );
	if ( *img == NULL )
	{
	}

	ClearImage(*img,0);


}


inline static void allocateOnDemand( IplImage **img, CvSize size, int depth, int channels )
{
	if ( *img != NULL )	
	{
		ClearImage(*img,0);
		return;
	}

	*img = cvCreateImage( size, depth, channels );
	if ( *img == NULL )
	{
	}

	ClearImage(*img,0);


}



public ref class CLROpticalFlow
{
public:
	IplImage* _frame1_1C;
	IplImage* _frame2_1C;

	IplImage* _OriginaImageColor;
	IplImage* _CompareImageColor;
	IplImage* _eig_image_32f;
	IplImage* _temp_image_32f;


	CvPoint2D32f * _frame1_features;
	CvPoint2D32f * _frame2_features;

	char * _optical_flow_found_feature;
	float*  _optical_flow_feature_error;

	CvSize  * _optical_flow_window;
	CvTermCriteria * _optical_flow_termination_criteria;

	IplImage * _pyramid1;
	IplImage * _pyramid2;

	IplImage *  _frame1;

	int _number_of_features;


	CLROpticalFlow()
	{
		_number_of_features = 10;
		_frame1_features = new CvPoint2D32f[_number_of_features];
		_frame2_features = new CvPoint2D32f[_number_of_features];

		_optical_flow_found_feature = new char[_number_of_features];
		_optical_flow_feature_error = new float[_number_of_features];

		_optical_flow_window = new CvSize;
		_optical_flow_window->width = 10;
		_optical_flow_window->height = 10;
		_optical_flow_termination_criteria = new CvTermCriteria;
		_optical_flow_termination_criteria->type = CV_TERMCRIT_ITER | CV_TERMCRIT_EPS;
		_optical_flow_termination_criteria->max_iter = 20;
		_optical_flow_termination_criteria->epsilon = .3;


	}

	bool Init(IplImage * initImage, CvSize * _size)
	{


		pin_ptr<IplImage*> p = &_frame1_1C;
		allocateOnDemand(p, *_size, IPL_DEPTH_8U, 1 );
		cvConvertImage(initImage, _frame1_1C, CV_BGR2GRAY);

		p = &_frame1;
		allocateOnDemand(p, *_size, IPL_DEPTH_8U, 3 );
		cvConvertImage(initImage, _frame1);

		p = &_frame2_1C;
		allocateOnDemand( p, *_size, IPL_DEPTH_8U, 1 );
		




		p = &_eig_image_32f;
		allocateOnDemand( p, *_size, IPL_DEPTH_32F, 1 );

		p = &_temp_image_32f;
		allocateOnDemand( p, *_size, IPL_DEPTH_32F, 1 );




		p = &_pyramid1;
		allocateOnDemand( p, *_size, IPL_DEPTH_8U, 1 );
		p = &_pyramid2;
		allocateOnDemand( p, *_size, IPL_DEPTH_8U, 1 );



		pin_ptr<int> pINT = &_number_of_features;
		cvGoodFeaturesToTrack(_frame1_1C, _eig_image_32f, _temp_image_32f, _frame1_features, pINT, .01, 10, NULL);
		

		return true;
	}


	bool Process(IplImage * pCurImage)
	{

		pin_ptr<int> pINT = &_number_of_features;
		cvGoodFeaturesToTrack(_frame1_1C, _eig_image_32f, _temp_image_32f, _frame1_features, pINT, .01, 10, NULL);
		

		cvConvertImage(pCurImage, _frame2_1C, CV_BGR2GRAY);

		cvCalcOpticalFlowPyrLK(_frame1_1C, _frame2_1C, _pyramid1, _pyramid2, _frame1_features, _frame2_features, _number_of_features, *_optical_flow_window, 5, _optical_flow_found_feature, _optical_flow_feature_error, *_optical_flow_termination_criteria, 0 );


		
		for(int i = 0; i < _number_of_features; i++)
		{
			/* If Pyramidal Lucas Kanade didn't really find the feature, skip it. */
			if ( _optical_flow_found_feature[i] == 0 )	continue;

			int line_thickness;				line_thickness = 1;
			/* CV_RGB(red, green, blue) is the red, green, and blue components
			* of the color you want, each out of 255.
			*/	
			CvScalar line_color;			line_color = CV_RGB(255,0,0);

			/* Let's make the flow field look nice with arrows. */

			/* The arrows will be a bit too short for a nice visualization because of the high framerate
			* (ie: there's not much motion between the frames).  So let's lengthen them by a factor of 3.
			*/
			CvPoint p,q;
			p.x = (int) _frame1_features[i].x;
			p.y = (int) _frame1_features[i].y;
			q.x = (int) _frame2_features[i].x;
			q.y = (int) _frame2_features[i].y;

			double angle;		angle = atan2( (double) p.y - q.y, (double) p.x - q.x );
			double hypotenuse;	hypotenuse = sqrt((double) (p.y - q.y)*(p.y - q.y) + (p.x - q.x)*(p.x - q.x) );

			/* Here we lengthen the arrow by a factor of three. */
			q.x = (int) (p.x - 3 * hypotenuse * cos(angle));
			q.y = (int) (p.y - 3 * hypotenuse * sin(angle));

			/* Now we draw the main line of the arrow. */
			/* "frame1" is the frame to draw on.
			* "p" is the point where the line begins.
			* "q" is the point where the line stops.
			* "CV_AA" means antialiased drawing.
			* "0" means no fractional bits in the center cooridinate or radius.
			*/
			cvLine( _frame2_1C, p, q, line_color, line_thickness, CV_AA, 0 );
			/* Now draw the tips of the arrow.  I do some scaling so that the
			* tips look proportional to the main line of the arrow.
			*/			
			p.x = (int) (q.x + 9 * cos(angle + pi / 4));
			p.y = (int) (q.y + 9 * sin(angle + pi / 4));
			cvLine( _frame2_1C, p, q, line_color, line_thickness, CV_AA, 0 );
			p.x = (int) (q.x + 9 * cos(angle - pi / 4));
			p.y = (int) (q.y + 9 * sin(angle - pi / 4));
			cvLine( _frame2_1C, p, q, line_color, line_thickness, CV_AA, 0 );

		}
	
		SwapImage(IplImage,_frame1_1C,_frame2_1C);
		SwapImage(IplImage,_pyramid1,_pyramid2);
		SwapImage(CvPoint2D32f,_frame1_features,_frame2_features);

		return true;
	}

};

