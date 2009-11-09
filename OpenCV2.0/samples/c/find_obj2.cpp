#include <cv.h>
#include <highgui.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <vector>
#include "find_obj2.h"

using namespace std;




void OnDetectcorner()
{
	double minVal=0.0, maxVal=0.0;
	double scale, shift;
	double min=0, max=255;
	int blocksize=3;

	IplImage* tmpImage = cvLoadImage( "box.png", CV_LOAD_IMAGE_GRAYSCALE );


	
	IplImage* harrisCorner=cvCreateImage(cvSize(tmpImage->width, tmpImage->height),IPL_DEPTH_32F,1);

	IplImage* output=cvCreateImage(cvSize(tmpImage->width, tmpImage->height),8,1);

	IplImage* visited=cvCreateImage(cvSize(tmpImage->width, tmpImage->height),8,1);
	cvZero(visited);

	cvCornerHarris(tmpImage,harrisCorner,blocksize);

	/*// Using 1 of the 3
	1. cvCornerHarris( input, output, 3, 5, 0.04);
	2. cvPreCornerDetect(input, output, 3);
	3. cvSobel(input, output, 0, 1, 3);
	*/


	cvMinMaxLoc( harrisCorner, &minVal, &maxVal, NULL, NULL, 0);

	scale = (max - min)/(maxVal-minVal);
	shift = -minVal * scale + min;
	cvConvertScale( harrisCorner, output, scale, shift );

	cvSaveImage("box_output.jpg",output);

}