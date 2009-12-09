#include "_cv.h"


struct CvSurfHF
{
    int p0, p1, p2, p3;
    float w;
};

CV_INLINE float
icvCalcHaarPattern( const int* origin, const CvSurfHF* f, int n )
{
    double d = 0;
    for( int k = 0; k < n; k++ )
        d += (origin[f[k].p0] + origin[f[k].p3] - origin[f[k].p1] - origin[f[k].p2])*f[k].w;
    return (float)d;
}

static void
icvResizeHaarPattern( const int src[][5], CvSurfHF* dst, int n, int oldSize, int newSize, int widthStep )
{
    float ratio = (float)newSize/oldSize;
    for( int k = 0; k < n; k++ )
    {
        int dx1 = cvRound( ratio*src[k][0] );
        int dy1 = cvRound( ratio*src[k][1] );
        int dx2 = cvRound( ratio*src[k][2] );
        int dy2 = cvRound( ratio*src[k][3] );
        dst[k].p0 = dy1*widthStep + dx1;
        dst[k].p1 = dy2*widthStep + dx1;
        dst[k].p2 = dy1*widthStep + dx2;
        dst[k].p3 = dy2*widthStep + dx2;
        dst[k].w = src[k][4]/((float)(dx2-dx1)*(dy2-dy1));
    }
}

/*
 * Maxima location interpolation as described in "Invariant Features from
 * Interest Point Groups" by Matthew Brown and David Lowe. This is performed by
 * fitting a 3D quadratic to a set of neighbouring samples.
 * 
 * The gradient vector and Hessian matrix at the initial keypoint location are 
 * approximated using central differences. The linear system Ax = b is then
 * solved, where A is the Hessian, b is the negative gradient, and x is the 
 * offset of the interpolated maxima coordinates from the initial estimate.
 * This is equivalent to an iteration of Netwon's optimisation algorithm.
 *
 * N9 contains the samples in the 3x3x3 neighbourhood of the maxima
 * dx is the sampling step in x
 * dy is the sampling step in y
 * ds is the sampling step in size
 * point contains the keypoint coordinates and scale to be modified
 *
 * Return value is 1 if interpolation was successful, 0 on failure.
 */   
CV_INLINE int 
icvInterpolateKeypoint( float N9[3][9], int dx, int dy, int ds, CvSURFPoint *point )
{
    int solve_ok;
    float A[9], x[3], b[3];
    CvMat _A = cvMat(3, 3, CV_32F, A);
    CvMat _x = cvMat(3, 1, CV_32F, x);                
    CvMat _b = cvMat(3, 1, CV_32F, b);

    b[0] = -(N9[1][5]-N9[1][3])/2;  /* Negative 1st deriv with respect to x */
    b[1] = -(N9[1][7]-N9[1][1])/2;  /* Negative 1st deriv with respect to y */
    b[2] = -(N9[2][4]-N9[0][4])/2;  /* Negative 1st deriv with respect to s */

    A[0] = N9[1][3]-2*N9[1][4]+N9[1][5];            /* 2nd deriv x, x */
    A[1] = (N9[1][8]-N9[1][6]-N9[1][2]+N9[1][0])/4; /* 2nd deriv x, y */
    A[2] = (N9[2][5]-N9[2][3]-N9[0][5]+N9[0][3])/4; /* 2nd deriv x, s */
    A[3] = A[1];                                    /* 2nd deriv y, x */
    A[4] = N9[1][1]-2*N9[1][4]+N9[1][7];            /* 2nd deriv y, y */
    A[5] = (N9[2][7]-N9[2][1]-N9[0][7]+N9[0][1])/4; /* 2nd deriv y, s */
    A[6] = A[2];                                    /* 2nd deriv s, x */
    A[7] = A[5];                                    /* 2nd deriv s, y */
    A[8] = N9[0][4]-2*N9[1][4]+N9[2][4];            /* 2nd deriv s, s */

    solve_ok = cvSolve( &_A, &_b, &_x );
    if( solve_ok )
    {
        point->pt.x += x[0]*dx;
        point->pt.y += x[1]*dy;
        point->size = cvRound( point->size + x[2]*ds ); 
    }
    return solve_ok;
}


/* Wavelet size at first layer of first octave. */ 
const int HAAR_SIZE0 = 9;    

/* Wavelet size increment between layers. This should be an even number, 
 such that the wavelet sizes in an octave are either all even or all odd.
 This ensures that when looking for the neighbours of a sample, the layers
 above and below are aligned correctly. */
const int HAAR_SIZE_INC = 6;


namespace cv
{

CvMat** dets = NULL;
CvMat** traces = NULL;

int _DetectorW = 0;
int _DetectorH = 0;

CvSeq* SURF2::icvFastHessianDetector( const CvMat* sum, const CvMat* mask_sum,
		CvMemStorage* storage, const CvSURFParams* params )
{
    CvSeq* points = cvCreateSeq( 0, sizeof(CvSeq), sizeof(CvSURFPoint), storage );

    /* Sampling step along image x and y axes at first octave. This is doubled
       for each additional octave. WARNING: Increasing this improves speed, 
       however keypoint extraction becomes unreliable. */
    const int SAMPLE_STEP0 = 1; 


    /* Wavelet Data */
    const int NX=3, NY=3, NXY=4, NM=1;
    static const int dx_s[NX][5] = { {0, 2, 3, 7, 1}, {3, 2, 6, 7, -2}, {6, 2, 9, 7, 1} };
    static const int dy_s[NY][5] = { {2, 0, 7, 3, 1}, {2, 3, 7, 6, -2}, {2, 6, 7, 9, 1} };
    static const int dxy_s[NXY][5] = { {1, 1, 4, 4, 1}, {5, 1, 8, 4, -1}, {1, 5, 4, 8, -1}, {5, 5, 8, 8, 1} };
    static const int dm[NM][5] = { {0, 0, 9, 9, 1} };
    CvSurfHF Dx[NX], Dy[NY], Dxy[NXY], Dm;

    
	if( dets == NULL)
	{
		dets = new CvMat * [params->nOctaveLayers+2];
	}
	
    
	if( traces == NULL)
	{
		traces = new CvMat * [params->nOctaveLayers+2];
	}
	

    int *sizes = (int*)cvStackAlloc((params->nOctaveLayers+2)*sizeof(sizes[0]));

    double dx = 0, dy = 0, dxy = 0;
    int octave, layer, sampleStep, size, margin;
    int rows, cols;
    int i, j, sum_i, sum_j;
    const int* s_ptr;
    float *det_ptr, *trace_ptr;

    /* Allocate enough space for hessian determinant and trace matrices at the 
       first octave. Clearing these initially or between octaves is not
       required, since all values that are accessed are first calculated */

	if( 	_DetectorW == 0)
	{
		for( layer = 0; layer <= params->nOctaveLayers+1; layer++ )
		{
			dets[layer]   = cvCreateMat( (sum->rows-1)/SAMPLE_STEP0, (sum->cols-1)/SAMPLE_STEP0, CV_32FC1 );
			traces[layer] = cvCreateMat( (sum->rows-1)/SAMPLE_STEP0, (sum->cols-1)/SAMPLE_STEP0, CV_32FC1 );
		}

		_DetectorW = sum->cols;
		_DetectorH = sum->rows;

	}else if( 	_DetectorW != sum->cols || _DetectorH != sum->rows)
	{

		_DetectorW = sum->cols;
		_DetectorH = sum->rows;



		for( layer = 0; layer <= params->nOctaveLayers+1; layer++ )
		{
			cvReleaseMat( &dets[layer] );
			cvReleaseMat( &traces[layer] );

			dets[layer]   = cvCreateMat( (sum->rows-1)/SAMPLE_STEP0, (sum->cols-1)/SAMPLE_STEP0, CV_32FC1 );
			traces[layer] = cvCreateMat( (sum->rows-1)/SAMPLE_STEP0, (sum->cols-1)/SAMPLE_STEP0, CV_32FC1 );
		}
	}


    for( octave = 0, sampleStep=SAMPLE_STEP0; octave < params->nOctaves; octave++, sampleStep*=2 )
    {
        /* Hessian determinant and trace sample array size in this octave */
        rows = (sum->rows-1)/sampleStep;
        cols = (sum->cols-1)/sampleStep;

        /* Calculate the determinant and trace of the hessian */
        for( layer = 0; layer <= params->nOctaveLayers+1; layer++ )
        {
            sizes[layer] = size = (HAAR_SIZE0+HAAR_SIZE_INC*layer)<<octave;
            icvResizeHaarPattern( dx_s, Dx, NX, 9, size, sum->cols );
            icvResizeHaarPattern( dy_s, Dy, NY, 9, size, sum->cols );
            icvResizeHaarPattern( dxy_s, Dxy, NXY, 9, size, sum->cols );
            /*printf( "octave=%d layer=%d size=%d rows=%d cols=%d\n", octave, layer, size, rows, cols );*/
            
            margin = (size/2)/sampleStep;
            for( sum_i=0, i=margin; sum_i<=(sum->rows-1)-size; sum_i+=sampleStep, i++ )
            {
                s_ptr = sum->data.i + sum_i*sum->cols;
                det_ptr = dets[layer]->data.fl + i*dets[layer]->cols + margin;
                trace_ptr = traces[layer]->data.fl + i*traces[layer]->cols + margin;
                for( sum_j=0, j=margin; sum_j<=(sum->cols-1)-size; sum_j+=sampleStep, j++ )
                {
                    dx  = icvCalcHaarPattern( s_ptr, Dx, 3 );
                    dy  = icvCalcHaarPattern( s_ptr, Dy, 3 );
                    dxy = icvCalcHaarPattern( s_ptr, Dxy, 4 );
                    s_ptr+=sampleStep;
                    *det_ptr++ = (float)(dx*dy - 0.81*dxy*dxy);
                    *trace_ptr++ = (float)(dx + dy);
                }
            }
        }

        /* Find maxima in the determinant of the hessian */
        for( layer = 1; layer <= params->nOctaveLayers; layer++ )
        {
            size = sizes[layer];
            icvResizeHaarPattern( dm, &Dm, NM, 9, size, mask_sum ? mask_sum->cols : sum->cols );
            
            /* Ignore pixels without a 3x3 neighbourhood in the layer above */
            margin = (sizes[layer+1]/2)/sampleStep+1; 
            for( i = margin; i < rows-margin; i++ )
            {
                det_ptr = dets[layer]->data.fl + i*dets[layer]->cols;
                trace_ptr = traces[layer]->data.fl + i*traces[layer]->cols;
                for( j = margin; j < cols-margin; j++ )
                {
                    float val0 = det_ptr[j];
                    if( val0 > params->hessianThreshold )
                    {
                        /* Coordinates for the start of the wavelet in the sum image. There   
                           is some integer division involved, so don't try to simplify this
                           (cancel out sampleStep) without checking the result is the same */
                        int sum_i = sampleStep*(i-(size/2)/sampleStep);
                        int sum_j = sampleStep*(j-(size/2)/sampleStep);

                        /* The 3x3x3 neighbouring samples around the maxima. 
                           The maxima is included at N9[1][4] */
                        int c = dets[layer]->cols;
                        const float *det1 = dets[layer-1]->data.fl + i*c + j;
                        const float *det2 = dets[layer]->data.fl   + i*c + j;
                        const float *det3 = dets[layer+1]->data.fl + i*c + j;
                        float N9[3][9] = { { det1[-c-1], det1[-c], det1[-c+1],          
                                             det1[-1]  , det1[0] , det1[1],
                                             det1[c-1] , det1[c] , det1[c+1]  },
                                           { det2[-c-1], det2[-c], det2[-c+1],       
                                             det2[-1]  , det2[0] , det2[1],
                                             det2[c-1] , det2[c] , det2[c+1 ] },
                                           { det3[-c-1], det3[-c], det3[-c+1],       
                                             det3[-1  ], det3[0] , det3[1],
                                             det3[c-1] , det3[c] , det3[c+1 ] } };

                        /* Check the mask - why not just check the mask at the center of the wavelet? */
                        if( mask_sum )
                        {
                            const int* mask_ptr = mask_sum->data.i +  mask_sum->cols*sum_i + sum_j;
                            float mval = icvCalcHaarPattern( mask_ptr, &Dm, 1 );
                            if( mval < 0.5 )
                                continue;
                        }

                        /* Non-maxima suppression. val0 is at N9[1][4]*/
                        if( val0 > N9[0][0] && val0 > N9[0][1] && val0 > N9[0][2] &&
                            val0 > N9[0][3] && val0 > N9[0][4] && val0 > N9[0][5] &&
                            val0 > N9[0][6] && val0 > N9[0][7] && val0 > N9[0][8] &&
                            val0 > N9[1][0] && val0 > N9[1][1] && val0 > N9[1][2] &&
                            val0 > N9[1][3]                    && val0 > N9[1][5] &&
                            val0 > N9[1][6] && val0 > N9[1][7] && val0 > N9[1][8] &&
                            val0 > N9[2][0] && val0 > N9[2][1] && val0 > N9[2][2] &&
                            val0 > N9[2][3] && val0 > N9[2][4] && val0 > N9[2][5] &&
                            val0 > N9[2][6] && val0 > N9[2][7] && val0 > N9[2][8] )
                        {
                            /* Calculate the wavelet center coordinates for the maxima */
                            double center_i = sum_i + (double)(size-1)/2;
                            double center_j = sum_j + (double)(size-1)/2;

                            CvSURFPoint point = cvSURFPoint( cvPoint2D32f(center_j,center_i), 
                                                             CV_SIGN(trace_ptr[j]), sizes[layer], 0, val0 );
                           
                            /* Interpolate maxima location within the 3x3x3 neighbourhood  */
                            int ds = sizes[layer]-sizes[layer-1];
                            int interp_ok = icvInterpolateKeypoint( N9, sampleStep, sampleStep, ds, &point );

                            /* Sometimes the interpolation step gives a negative size etc. */
                            if( interp_ok && point.size >= 1 &&
                                point.pt.x >= 0 && point.pt.x <= (sum->cols-1) &&
                                point.pt.y >= 0 && point.pt.y <= (sum->rows-1) )
                            {    
                                /*printf( "KeyPoint %f %f %d\n", point.pt.x, point.pt.y, point.size );*/
                                cvSeqPush( points, &point );
                            }    
                        }
                    }
                }
            }
        }
    }

    /* Clean-up */
    //for( layer = 0; layer <= params->nOctaveLayers+1; layer++ )
    //{
    //    cvReleaseMat( &dets[layer] );
    //    cvReleaseMat( &traces[layer] );
    //}

    return points;
}



	
double
compareSURFDescriptors( const float* d1, const float* d2, double best, int length )
{
    double total_cost = 0;
    assert( length % 4 == 0 );
    for( int i = 0; i < length; i += 4 )
    {
        double t0 = d1[i] - d2[i];
        double t1 = d1[i+1] - d2[i+1];
        double t2 = d1[i+2] - d2[i+2];
        double t3 = d1[i+3] - d2[i+3];
        total_cost += t0*t0 + t1*t1 + t2*t2 + t3*t3;
        if( total_cost > best )
            break;
    }
    return total_cost;
}


int
naiveNearestNeighbor( const float* vec, int laplacian,
                      const CvSeq* model_keypoints,
                      const CvSeq* model_descriptors )
{
    int length = (int)(model_descriptors->elem_size/sizeof(float));
    int i, neighbor = -1;
    double d, dist1 = 1e6, dist2 = 1e6;
    CvSeqReader reader, kreader;
    cvStartReadSeq( model_keypoints, &kreader, 0 );
    cvStartReadSeq( model_descriptors, &reader, 0 );

    for( i = 0; i < model_descriptors->total; i++ )
    {
        const CvSURFPoint* kp = (const CvSURFPoint*)kreader.ptr;
        const float* mvec = (const float*)reader.ptr;
    	CV_NEXT_SEQ_ELEM( kreader.seq->elem_size, kreader );
        CV_NEXT_SEQ_ELEM( reader.seq->elem_size, reader );
        if( laplacian != kp->laplacian )
            continue;
        d = compareSURFDescriptors( vec, mvec, dist2, length );
        if( d < dist1 )
        {
            dist2 = dist1;
            dist1 = d;
            neighbor = i;
        }
        else if ( d < dist2 )
            dist2 = d;
    }
    if ( dist1 < 0.6*dist2 )
        return neighbor;
    return -1;
}

void
findPairs( const CvSeq* objectKeypoints, const CvSeq* objectDescriptors,
           const CvSeq* imageKeypoints, const CvSeq* imageDescriptors, vector<int>& ptpairs )
{
    int i;
    CvSeqReader reader, kreader;
    cvStartReadSeq( objectKeypoints, &kreader );
    cvStartReadSeq( objectDescriptors, &reader );
    ptpairs.clear();

    for( i = 0; i < objectDescriptors->total; i++ )
    {
        const CvSURFPoint* kp = (const CvSURFPoint*)kreader.ptr;
        const float* descriptor = (const float*)reader.ptr;
        CV_NEXT_SEQ_ELEM( kreader.seq->elem_size, kreader );
        CV_NEXT_SEQ_ELEM( reader.seq->elem_size, reader );
        int nearest_neighbor = naiveNearestNeighbor( descriptor, kp->laplacian, imageKeypoints, imageDescriptors );
        if( nearest_neighbor >= 0 )
        {
            ptpairs.push_back(i);
            ptpairs.push_back(nearest_neighbor);
        }
    }
}


void
flannFindPairs( const CvSeq*, const CvSeq* objectDescriptors,
           const CvSeq*, const CvSeq* imageDescriptors, vector<int>& ptpairs )
{
	int length = (int)(objectDescriptors->elem_size/sizeof(float));

    cv::Mat m_object(objectDescriptors->total, length, CV_32F);
	cv::Mat m_image(imageDescriptors->total, length, CV_32F);


	// copy descriptors
    CvSeqReader obj_reader;
	float* obj_ptr = m_object.ptr<float>(0);
    cvStartReadSeq( objectDescriptors, &obj_reader );
    for(int i = 0; i < objectDescriptors->total; i++ )
    {
        const float* descriptor = (const float*)obj_reader.ptr;
        CV_NEXT_SEQ_ELEM( obj_reader.seq->elem_size, obj_reader );
        memcpy(obj_ptr, descriptor, length*sizeof(float));
        obj_ptr += length;
    }
    CvSeqReader img_reader;
	float* img_ptr = m_image.ptr<float>(0);
    cvStartReadSeq( imageDescriptors, &img_reader );
    for(int i = 0; i < imageDescriptors->total; i++ )
    {
        const float* descriptor = (const float*)img_reader.ptr;
        CV_NEXT_SEQ_ELEM( img_reader.seq->elem_size, img_reader );
        memcpy(img_ptr, descriptor, length*sizeof(float));
        img_ptr += length;
    }

    // find nearest neighbors using FLANN
    cv::Mat m_indices(objectDescriptors->total, 2, CV_32S);
    cv::Mat m_dists(objectDescriptors->total, 2, CV_32F);
    cv::flann::Index flann_index(m_image, cv::flann::KDTreeIndexParams(4));  // using 4 randomized kdtrees
    flann_index.knnSearch(m_object, m_indices, m_dists, 2, cv::flann::SearchParams(64) ); // maximum number of leafs checked

    int* indices_ptr = m_indices.ptr<int>(0);
    float* dists_ptr = m_dists.ptr<float>(0);
    for (int i=0;i<m_indices.rows;++i) {
    	if (dists_ptr[2*i]<0.6*dists_ptr[2*i+1]) {
    		ptpairs.push_back(i);
    		ptpairs.push_back(indices_ptr[2*i]);
    	}
    }
}





	void SURF2::InitResource()
	{
		_storage = cvCreateMemStorage(0);
		_params = cvSURFParams(500, 0);

		//_params = new CvSURFParams(500,1);
		_image = NULL;

	}

	void SURF2::ReleaseResource()
	{

	}

	void SURF2::ReSizeAllocateData(int w,int h)
	{

	}

	
	void SURF2::Init(IplImage* object)
	{
		if( _storage == NULL)
			InitResource();

		_ObjectX = object->width;
		_ObjectY = object->height;

		cvExtractSURF( object, 0, &_objectKeypoints, &_objectDescriptors, _storage, _params ,0);
	}


	void SURF2::Init(char * pImg,int w,int h)
	{
		IplImage* object = cvCreateImage(cvGetSize(object), 8, 3);
		
		cvExtractSURF( object, 0, &_objectKeypoints, &_objectDescriptors, _storage, _params ,0);
	}


	class ComparePoint
	{
	public:
		int _X;
		int _Y;
		int _Count;
	};

	bool  SURF2::Compare(IplImage* image_color)
	{
		
		    static CvScalar colors[] = 
			{
				{{0,0,255}},
				{{0,128,255}},
				{{0,255,255}},
				{{0,255,0}},
				{{255,128,0}},
				{{255,255,0}},
				{{255,0,0}},
				{{255,0,255}},
				{{255,255,255}}
			};
		 

		double scale;

		if(_image == NULL)
		{
			//_image = cvCreateImage(cvSize(160,120),8,1);
			//_imageResize = cvCreateImage(cvSize(160,120),8,3);
			_image = cvCreateImage(cvGetSize(image_color),8,1);
		}
		
		scale = 1;//image_color->width / 160;

		//cvResize(image_color,_imageResize,CV_INTER_CUBIC); 


		cvCvtColor( image_color, _image, CV_RGB2GRAY );


		//image = cvCreateImage(cvSize(w, h) , 8, 3);

		CvSeq *imageKeypoints = 0, *imageDescriptors = 0;


		CvMemStorage * s = cvCreateChildMemStorage(_storage);


		cvExtractSURF( _image, 0, &imageKeypoints, &imageDescriptors, s, _params ,0);


		


		this->_compareTime = (double) cvGetTickCount();

		vector<int> ptpairs;


		flannFindPairs( _objectKeypoints, _objectDescriptors, imageKeypoints, imageDescriptors, ptpairs );



		
		int n = ptpairs.size()/2;
		if( n < 4 )
		{


			cvClearMemStorage(s);
			cvReleaseMemStorage(&s);
			return 0;
		}


		vector<CvPoint2D32f> pt1, pt2;
		CvMat _pt1, _pt2;

		CvPoint dst_corners[4];
		

	    pt1.resize(n);
	    pt2.resize(n);

		double h[9];
		CvMat _h = cvMat(3, 3, CV_64F, h);


    


		vector<ComparePoint> cpArr;
		
		//int cx,cy;
		
		ComparePoint cp;

	    for(int i = 0; i < (int)ptpairs.size(); i += 2 )
		{
			CvSURFPoint* r1 = (CvSURFPoint*)cvGetSeqElem( _objectKeypoints, ptpairs[i] );
			CvSURFPoint* r2 = (CvSURFPoint*)cvGetSeqElem( imageKeypoints, ptpairs[i+1] );


			r2->pt.x *= scale;
			r2->pt.y *= scale;


			CvPoint center;
			int radius;
	        center.x = cvRound(r2->pt.x );
		    center.y = cvRound(r2->pt.y);
			radius = cvRound(r2->size*1.2/9.*2);

			cvCircle( image_color, center, radius, colors[0], 1, 8, 0 );

		
		
			pt1[i/2] = r1->pt;
			pt2[i/2] = r2->pt;
		

		}
		
		

		_pt1 = cvMat(1, n, CV_32FC2, &pt1[0] );
		_pt2 = cvMat(1, n, CV_32FC2, &pt2[0] );

		if( !cvFindHomography( &_pt1, &_pt2, &_h, CV_RANSAC, 5 ))
		{
			

			cvClearMemStorage(s);
			cvReleaseMemStorage(&s);
		
			return false;
		}

		CvPoint src_corners[4] = {{0,0}, {_ObjectX,0}, {_ObjectX, _ObjectY}, {0, _ObjectY}};

		for(int i = 0; i < 4; i++ )
		{
			double x = src_corners[i].x; 
			double y = src_corners[i].y;
			double Z = 1./(h[6]*x + h[7]*y + h[8]);
			double X = (h[0]*x + h[1]*y + h[2])*Z;
			double Y = (h[3]*x + h[4]*y + h[5])*Z;
			dst_corners[i] = cvPoint(cvRound(X), cvRound(Y)) ;
		}


		 for(int i = 0; i < 4; i++ )
        {
            CvPoint r1 = dst_corners[i%4];
            CvPoint r2 = dst_corners[(i+1)%4];
            cvLine( image_color, cvPoint(r1.x, r1.y),
                cvPoint(r2.x, r2.y ), colors[8] );
        }


		cvClearMemStorage(s);
		cvReleaseMemStorage(&s);
		

		this->_compareTime = (double) cvGetTickCount()  - this->_compareTime;
	
		 this->_compareTime = this->_compareTime/(cvGetTickFrequency()*1000.);



		 this->_CX = dst_corners[2].x /2;
		 this->_CY = dst_corners[2].y /2;


		 return true;

		//imageKeypoints->cl
		//icvReleaseSeq(
	}


	    /* Standard deviation of the Gaussian used to weight the gradient samples
       used to assign an orientation */ 
    const float ORI_SIGMA = 2.5f;



    /* Radius of the circle in which to sample gradients to assign an 
       orientation */
    const int ORI_RADIUS = 6; 
	

	    /* Coordinates and weights of samples used to calculate orientation */
    static cv::Mat _G = cv::getGaussianKernel( 2*ORI_RADIUS+1, ORI_SIGMA, CV_32F );
    static const float* G = (const float*)_G.data;

	int nangle0 = 0;
const int max_ori_samples = (2*ORI_RADIUS+1)*(2*ORI_RADIUS+1);
    CvPoint apt[max_ori_samples];
    float apt_w[max_ori_samples];


	    /* Standard deviation of the Gaussian used to weight the gradient samples
       used to build a keypoint descriptor */
    const float DESC_SIGMA = 3.3f;

	const int PATCH_SZ = 20;

    float DW[PATCH_SZ][PATCH_SZ];
    CvMat _DW = cvMat(PATCH_SZ, PATCH_SZ, CV_32F, DW);


void
SURF2::cvExtractSURF( const CvArr* _img, const CvArr* _mask,
               CvSeq** _keypoints, CvSeq** _descriptors,
               CvMemStorage* storage, CvSURFParams params,
			   int useProvidedKeyPts)
{

	this->_descriptorTime = (double)cvGetTickCount();


    CvMat /**sum = 0,*/ *mask1 = 0, *mask_sum = 0, **win_bufs = 0;

    if( _keypoints && !useProvidedKeyPts ) // If useProvidedKeyPts!=0 we'll use current contents of "*_keypoints"
        *_keypoints = 0;
    if( _descriptors )
        *_descriptors = 0;


    /* The size of the sliding window (in degrees) used to assign an 
       orientation */
    const int ORI_WIN = 60;   

    /* Increment used for the orientation sliding window (in degrees) */
    const int ORI_SEARCH_INC = 5;  




    /* X and Y gradient wavelet data */
    const int NX=2, NY=2;
    int dx_s[NX][5] = {{0, 0, 2, 4, -1}, {2, 0, 4, 4, 1}};
    int dy_s[NY][5] = {{0, 0, 4, 2, 1}, {0, 2, 4, 4, -1}};

    CvSeq *keypoints, *descriptors = 0;
    CvMat imghdr, *img = cvGetMat(_img, &imghdr);
    CvMat maskhdr, *mask = _mask ? cvGetMat(_mask, &maskhdr) : 0;
    
    
    int descriptor_size = params.extended ? 128 : 64;
    const int descriptor_data_type = CV_32F;
    
    int i, j, k,  N;
    int nthreads = cvGetNumThreads();

    CV_Assert(img != 0);
    CV_Assert(CV_MAT_TYPE(img->type) == CV_8UC1);
    CV_Assert(mask == 0 || (CV_ARE_SIZES_EQ(img,mask) && CV_MAT_TYPE(mask->type) == CV_8UC1));
    CV_Assert(storage != 0);
    CV_Assert(params.hessianThreshold >= 0);
    CV_Assert(params.nOctaves > 0);
    CV_Assert(params.nOctaveLayers > 0);

	if( _sum ==NULL)
	{
		_sum = cvCreateMat( img->height+1, img->width+1, CV_32SC1 );
	}else if( _sum->height != (img->height+1) || _sum->width != (img->width+1))
	{
		cvReleaseMat(&_sum);
		_sum = cvCreateMat( img->height+1, img->width+1, CV_32SC1 );
	}

    cvIntegral( img, _sum );
	
	// Compute keypoints only if we are not asked for evaluating the descriptors are some given locations:
	if (!useProvidedKeyPts)
	{
		if( mask )
		{
			mask1 = cvCreateMat( img->height, img->width, CV_8UC1 );
			mask_sum = cvCreateMat( img->height+1, img->width+1, CV_32SC1 );
			cvMinS( mask, 1, mask1 );
			cvIntegral( mask1, mask_sum );
		}

		this->_extremaTime = (double)cvGetTickCount();

		keypoints = icvFastHessianDetector( _sum, mask_sum, storage, &params );

		this->_extremaTime = (double)cvGetTickCount() - this->_extremaTime;


		this->_extremaTime = this->_extremaTime/(cvGetTickFrequency()*1000.);


	}
	else
	{
		CV_Assert(useProvidedKeyPts && (_keypoints != 0) && (*_keypoints != 0));
		keypoints = *_keypoints;
	}

    N = keypoints->total;
    if( _descriptors )
    {
        descriptors = cvCreateSeq( 0, sizeof(CvSeq),
            descriptor_size*CV_ELEM_SIZE(descriptor_data_type), storage );
        cvSeqPushMulti( descriptors, 0, N );
    }

    
	if( nangle0 == 0)
	{
		for( i = -ORI_RADIUS; i <= ORI_RADIUS; i++ )
		{
			for( j = -ORI_RADIUS; j <= ORI_RADIUS; j++ )
			{
				if( i*i + j*j <= ORI_RADIUS*ORI_RADIUS )
				{
					apt[nangle0] = cvPoint(j,i);
					apt_w[nangle0++] = G[i+ORI_RADIUS]*G[j+ORI_RADIUS];
				}
			}
		}

	

		/* Gaussian used to weight descriptor samples */
		{
		double c2 = 1./(DESC_SIGMA*DESC_SIGMA*2);
		double gs = 0;
		for( i = 0; i < PATCH_SZ; i++ )
		{
			for( j = 0; j < PATCH_SZ; j++ )
			{
				double x = j - (float)(PATCH_SZ-1)/2, y = i - (float)(PATCH_SZ-1)/2;
				double val = exp(-(x*x+y*y)*c2);
				DW[i][j] = (float)val;
				gs += val;
			}
		}
		cvScale( &_DW, &_DW, 1./gs );
		}
	}



    win_bufs = (CvMat**)cvAlloc(nthreads*sizeof(win_bufs[0]));
    for( i = 0; i < nthreads; i++ )
        win_bufs[i] = 0;




	        float X[max_ori_samples], Y[max_ori_samples], angle[max_ori_samples];
        uchar PATCH[PATCH_SZ+1][PATCH_SZ+1];
        float DX[PATCH_SZ][PATCH_SZ], DY[PATCH_SZ][PATCH_SZ];
        CvMat _X = cvMat(1, max_ori_samples, CV_32F, X);
        CvMat _Y = cvMat(1, max_ori_samples, CV_32F, Y);
        CvMat _angle = cvMat(1, max_ori_samples, CV_32F, angle);
        CvMat _patch = cvMat(PATCH_SZ+1, PATCH_SZ+1, CV_8U, PATCH);
        float* vec;
        CvSurfHF dx_t[NX], dy_t[NY];


#ifdef _OPENMP
#pragma omp parallel for num_threads(nthreads) schedule(dynamic)
#endif
    for( k = 0; k < N; k++ )
    {
        const int* sum_ptr = _sum->data.i;
        int sum_cols = _sum->cols;
        int i, j, kk, x, y, nangle;

		int thread_idx = cvGetThreadNum();
        
        CvSURFPoint* kp = (CvSURFPoint*)cvGetSeqElem( keypoints, k );
        int size = kp->size;
        CvPoint2D32f center = kp->pt;

        /* The sampling intervals and wavelet sized for selecting an orientation
           and building the keypoint descriptor are defined relative to 's' */
        float s = (float)size*1.2f/9.0f;

        /* To find the dominant orientation, the gradients in x and y are
           sampled in a circle of radius 6s using wavelets of size 4s.
           We ensure the gradient wavelet size is even to ensure the 
           wavelet pattern is balanced and symmetric around its center */
        int grad_wav_size = 2*cvRound( 2*s );
        if ( _sum->rows < grad_wav_size || _sum->cols < grad_wav_size )
        {
            /* when grad_wav_size is too big,
	     * the sampling of gradient will be meaningless
	     * mark keypoint for deletion. */
            kp->size = -1;
            continue;
        }
        icvResizeHaarPattern( dx_s, dx_t, NX, 4, grad_wav_size, _sum->cols );
        icvResizeHaarPattern( dy_s, dy_t, NY, 4, grad_wav_size, _sum->cols );
        for( kk = 0, nangle = 0; kk < nangle0; kk++ )
        {
            const int* ptr;
            float vx, vy;
            x = cvRound( center.x + apt[kk].x*s - (float)(grad_wav_size-1)/2 );
            y = cvRound( center.y + apt[kk].y*s - (float)(grad_wav_size-1)/2 );
            if( (unsigned)y >= (unsigned)(_sum->rows - grad_wav_size) ||
                (unsigned)x >= (unsigned)(_sum->cols - grad_wav_size) )
                continue;
            ptr = sum_ptr + x + y*sum_cols;
            vx = icvCalcHaarPattern( ptr, dx_t, 2 );
            vy = icvCalcHaarPattern( ptr, dy_t, 2 );
            X[nangle] = vx*apt_w[kk]; Y[nangle] = vy*apt_w[kk];
            nangle++;
        }
        if ( nangle == 0 )
        {
            /* No gradient could be sampled because the keypoint is too
	     * near too one or more of the sides of the image. As we
	     * therefore cannot find a dominant direction, we skip this
	     * keypoint and mark it for later deletion from the sequence. */
            kp->size = -1;
            continue;
        }
        _X.cols = _Y.cols = _angle.cols = nangle;
        cvCartToPolar( &_X, &_Y, 0, &_angle, 1 );

        float bestx = 0, besty = 0, descriptor_mod = 0;
        for( i = 0; i < 360; i += ORI_SEARCH_INC )
        {
            float sumx = 0, sumy = 0, temp_mod;
            for( j = 0; j < nangle; j++ )
            {
				int d = std::abs(cvRound(angle[j]) - i);
                if( d < ORI_WIN/2 || d > 360-ORI_WIN/2 )
                {
                    sumx += X[j];
                    sumy += Y[j];
                }
            }
            temp_mod = sumx*sumx + sumy*sumy;
            if( temp_mod > descriptor_mod )
            {
                descriptor_mod = temp_mod;
                bestx = sumx;
                besty = sumy;
            }
        }
        
        float descriptor_dir = cvFastArctan( besty, bestx );
        kp->dir = descriptor_dir;

        if( !_descriptors )
            continue;

        descriptor_dir *= (float)(CV_PI/180);
        
        /* Extract a window of pixels around the keypoint of size 20s */
        int win_size = (int)((PATCH_SZ+1)*s);
        if( win_bufs[thread_idx] == 0 || win_bufs[thread_idx]->cols < win_size*win_size )
        {
            cvReleaseMat( &win_bufs[thread_idx] );
            win_bufs[thread_idx] = cvCreateMat( 1, win_size*win_size, CV_8U );
        }
        
        CvMat win = cvMat(win_size, win_size, CV_8U, win_bufs[thread_idx]->data.ptr);
        float sin_dir = sin(descriptor_dir);
        float cos_dir = cos(descriptor_dir) ;

        /* Subpixel interpolation version (slower). Subpixel not required since
           the pixels will all get averaged when we scale down to 20 pixels */
        /*  
        float w[] = { cos_dir, sin_dir, center.x,
                      -sin_dir, cos_dir , center.y };
        CvMat W = cvMat(2, 3, CV_32F, w);
        cvGetQuadrangleSubPix( img, &win, &W );
        */

        /* Nearest neighbour version (faster) */
        float win_offset = -(float)(win_size-1)/2;
        float start_x = center.x + win_offset*cos_dir + win_offset*sin_dir;
        float start_y = center.y - win_offset*sin_dir + win_offset*cos_dir;
        uchar* WIN = win.data.ptr;
        for( i=0; i<win_size; i++, start_x+=sin_dir, start_y+=cos_dir )
        {
            float pixel_x = start_x;
            float pixel_y = start_y;
            for( j=0; j<win_size; j++, pixel_x+=cos_dir, pixel_y-=sin_dir )
            {
                int x = cvRound( pixel_x );
                int y = cvRound( pixel_y );
                x = MAX( x, 0 );
                y = MAX( y, 0 );
                x = MIN( x, img->cols-1 );
                y = MIN( y, img->rows-1 );
                WIN[i*win_size + j] = img->data.ptr[y*img->step+x];
             }
        }

        /* Scale the window to size PATCH_SZ so each pixel's size is s. This
           makes calculating the gradients with wavelets of size 2s easy */
        cvResize( &win, &_patch, CV_INTER_AREA );

        /* Calculate gradients in x and y with wavelets of size 2s */
        for( i = 0; i < PATCH_SZ; i++ )
            for( j = 0; j < PATCH_SZ; j++ )
            {
                float dw = DW[i][j];
                float vx = (PATCH[i][j+1] - PATCH[i][j] + PATCH[i+1][j+1] - PATCH[i+1][j])*dw;
                float vy = (PATCH[i+1][j] - PATCH[i][j] + PATCH[i+1][j+1] - PATCH[i][j+1])*dw;
                DX[i][j] = vx;
                DY[i][j] = vy;
            }

        /* Construct the descriptor */
        vec = (float*)cvGetSeqElem( descriptors, k );
        for( kk = 0; kk < (int)(descriptors->elem_size/sizeof(vec[0])); kk++ )
            vec[kk] = 0;
        double square_mag = 0;       
        if( params.extended )
        {
            /* 128-bin descriptor */
            for( i = 0; i < 4; i++ )
                for( j = 0; j < 4; j++ )
                {
                    for( y = i*5; y < i*5+5; y++ )
                    {
                        for( x = j*5; x < j*5+5; x++ )
                        {
                            float tx = DX[y][x], ty = DY[y][x];
                            if( ty >= 0 )
                            {
                                vec[0] += tx;
                                vec[1] += (float)fabs(tx);
                            } else {
                                vec[2] += tx;
                                vec[3] += (float)fabs(tx);
                            }
                            if ( tx >= 0 )
                            {
                                vec[4] += ty;
                                vec[5] += (float)fabs(ty);
                            } else {
                                vec[6] += ty;
                                vec[7] += (float)fabs(ty);
                            }
                        }
                    }
                    for( kk = 0; kk < 8; kk++ )
                        square_mag += vec[kk]*vec[kk];
                    vec += 8;
                }
        }
        else
        {
            /* 64-bin descriptor */
            for( i = 0; i < 4; i++ )
                for( j = 0; j < 4; j++ )
                {
                    for( y = i*5; y < i*5+5; y++ )
                    {
                        for( x = j*5; x < j*5+5; x++ )
                        {
                            float tx = DX[y][x], ty = DY[y][x];
                            vec[0] += tx; vec[1] += ty;
                            vec[2] += (float)fabs(tx); vec[3] += (float)fabs(ty);
                        }
                    }
                    for( kk = 0; kk < 4; kk++ )
                        square_mag += vec[kk]*vec[kk];
                    vec+=4;
                }
        }

        /* unit vector is essential for contrast invariance */
        vec = (float*)cvGetSeqElem( descriptors, k );
        double scale = 1./(sqrt(square_mag) + DBL_EPSILON);
        for( kk = 0; kk < descriptor_size; kk++ )
            vec[kk] = (float)(vec[kk]*scale);
    }
    
    /* remove keypoints that were marked for deletion */
    for ( i = 0; i < N; i++ )
    {
        CvSURFPoint* kp = (CvSURFPoint*)cvGetSeqElem( keypoints, i );
        if ( kp->size == -1 )
        {
            cvSeqRemove( keypoints, i );
            if ( _descriptors )
                cvSeqRemove( descriptors, i );
            k--;
	    N--;
        }
    }

    for( i = 0; i < nthreads; i++ )
        cvReleaseMat( &win_bufs[i] );

    if( _keypoints && !useProvidedKeyPts )
        *_keypoints = keypoints;
    if( _descriptors )
        *_descriptors = descriptors;

    //cvReleaseMat( &sum );
    if (mask1) cvReleaseMat( &mask1 );
    if (mask_sum) cvReleaseMat( &mask_sum );
    cvFree( &win_bufs );


	this->_descriptorTime = (double)cvGetTickCount() -this->_descriptorTime;

	this->_descriptorTime -= this->_extremaTime;

	this->_descriptorTime = this->_descriptorTime/(cvGetTickFrequency()*1000.);

}

}
