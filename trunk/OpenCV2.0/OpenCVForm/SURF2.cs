using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using OpenCvSharp;


namespace OpenCVForm
{

    public class  CvSurfHF
    {
        public int p0;
        public int p1;
        public int p2;
        public int p3;
        public float w;
    };


    class SURF2
    {




        List<float[]> flannFindPairs(List<CvSURFPoint> objectKeypoints, List<float[]> objectDescriptors, List<CvSURFPoint> imageKeypoints, List<float[]> imageDescriptors)
        {



            return null;
        }


        int[] integralMap;
        int _Width;
        int _Height;
        
        float [] getGaussianKernel( int n, double sigma)
        {
            //const int SMALL_GAUSSIAN_SIZE = 7;
            //float [][] small_gaussian_tab = new float [][]  {
            //    new float [] {1},
            //    new float [] {0.25f, 0.5f, 0.25f},
            //    new float [] {0.0625f, 0.25f, 0.375f, 0.25f, 0.0625f},
            //    new float [] {0.03125f, 0.109375f, 0.21875f, 0.28125f, 0.21875f, 0.109375f, 0.03125f}
            //};

            float [] cf = new float [n];
        

            double sigmaX = sigma > 0 ? sigma : ((n-1)*0.5 - 1)*0.3 + 0.8;
            double scale2X = -0.5/(sigmaX*sigmaX);
            double sum = 0;

            int i;
            for( i = 0; i < n; i++ )
            {
                double x = i - (n-1)*0.5;


                {
                    cf[i] = (float)Math.Exp(scale2X*x*x);
                    sum += cf[i];
                }
            }

            sum = 1/sum;
            for( i = 0; i < n; i++ )
            {
                
                cf[i] = (float)(cf[i]*sum);
            }

            return cf;
        }


        bool extended = true;

        unsafe public List<float[]> MakeDiscriptor(Bitmap ibmp, List<CvSURFPoint> keypoints)
        {


            Rectangle rect = new Rectangle(0, 0, ibmp.Width, ibmp.Height);
            System.Drawing.Imaging.BitmapData data = ibmp.LockBits(rect, System.Drawing.Imaging.ImageLockMode.ReadWrite,
                ibmp.PixelFormat);
            CvMemStorage storage = Cv.CreateMemStorage();

            //const int descriptor_data_type = CV_32F;
            
            int descriptor_size = extended ? 128 : 64;


            //CvSeq descriptors = Cv.CreateSeq(SeqType.Zero, CvSeq.SizeOf, descriptor_size * CvSeq<float>.SizeOf, storage);
            List<float[]> descriptors = new List<float[]>();


            //descriptors = (CvSeq) 

            //float [] fZero = new float [1];
            //fZero[0] = 0;

            //Cv.SeqPushMulti<float>(descriptors,fZero, InsertPosition.Back);

        //descriptors = cvCreateSeq( 0, sizeof(CvSeq),
        //    descriptor_size*CV_ELEM_SIZE(descriptor_data_type), storage );
        //cvSeqPushMulti( descriptors, 0, N );


            const int ORI_RADIUS = 6;
            const float ORI_SIGMA = 2.5f;

            float[] G = getGaussianKernel(2 * ORI_RADIUS + 1, ORI_SIGMA);

            const int max_ori_samples = (2 * ORI_RADIUS + 1) * (2 * ORI_RADIUS + 1);
            CvPoint[] apt = new CvPoint[max_ori_samples];
            float[] apt_w = new float[max_ori_samples];

            int nangle0 = 0;

            for (int i = -ORI_RADIUS; i <= ORI_RADIUS; i++)
            {
                for (int j = -ORI_RADIUS; j <= ORI_RADIUS; j++)
                {
                    if (i * i + j * j <= ORI_RADIUS * ORI_RADIUS)
                    {
                        apt[nangle0] = new CvPoint(j, i);
                        apt_w[nangle0++] = G[i + ORI_RADIUS] * G[j + ORI_RADIUS];
                    }
                }
            }

            const int PATCH_SZ = 20;

            float[,] DW = new float[PATCH_SZ, PATCH_SZ];

            /* Standard deviation of the Gaussian used to weight the gradient samples
            used to build a keypoint descriptor */

            const float DESC_SIGMA = 3.3f;

            CvMat _DW = new CvMat(PATCH_SZ, PATCH_SZ, MatrixType.F32C1, DW);

            /* Gaussian used to weight descriptor samples */
            {
                double c2 = 1 / (DESC_SIGMA * DESC_SIGMA * 2);
                double gs = 0;
                for (int i = 0; i < PATCH_SZ; i++)
                {
                    for (int j = 0; j < PATCH_SZ; j++)
                    {
                        double x = j - (float)(PATCH_SZ - 1) / 2, y = i - (float)(PATCH_SZ - 1) / 2;
                        double val = Math.Exp(-(x * x + y * y) * c2);
                        DW[i, j] = (float)val;
                        gs += val;
                    }
                }
                Cv.Scale(_DW, _DW, 1 / gs);




            }

            const int NX=2, NY=2;
            int [,] dx_s = {{0, 0, 2, 4, -1}, {2, 0, 4, 4, 1}};
            int [,] dy_s = {{0, 0, 4, 2, 1}, {0, 2, 4, 4, -1}};

            
            /* Increment used for the orientation sliding window (in degrees) */
            const int ORI_SEARCH_INC = 5; 
                /* The size of the sliding window (in degrees) used to assign an 
               orientation */
            const int ORI_WIN = 60;   


            CvMat win_bufs = null;


            fixed (int* imgPtr = integralMap)
            {
                byte* img = (byte*)(data.Scan0);


            for(int k = 0; k < keypoints.Count; k++ )
            {
                //const int* sum_ptr = sum->data.i;
                int sum_cols = _Width;
                int i, j, kk, x, y, nangle;
                float [] X = new float [max_ori_samples];
                float [] Y = new float [max_ori_samples];
                float [] angle = new float [max_ori_samples];

                //uchar PATCH[PATCH_SZ+1][PATCH_SZ+1];
                byte [,] PATCH = new byte [PATCH_SZ+1,PATCH_SZ+1];

                float [,] DX = new float [PATCH_SZ,PATCH_SZ];
                float [,] DY = new float [PATCH_SZ,PATCH_SZ];

                CvMat _X = new CvMat(1, max_ori_samples,  MatrixType.F32C1, X);
                CvMat _Y = new CvMat(1, max_ori_samples,  MatrixType.F32C1, Y);
                CvMat _angle =new CvMat(1, max_ori_samples,  MatrixType.F32C1, angle);

                CvMat _patch = new CvMat(PATCH_SZ+1, PATCH_SZ+1,MatrixType.U8C1, PATCH);

                //float* vec;

                CvSurfHF [] dx_t = new CvSurfHF [NX];
                CvSurfHF [] dy_t = new CvSurfHF [NY];

                //int thread_idx = cvGetThreadNum();
                
                CvSURFPoint kp = keypoints[k];//(CvSURFPoint)Cv.GetSeqElem<CvSURFPoint>( keypoints, k );

                int size = kp.Size;

                CvPoint2D32f center = kp.Pt;

                /* The sampling intervals and wavelet sized for selecting an orientation
                   and building the keypoint descriptor are defined relative to 's' */
                float s = (float)size*1.2f/9.0f;

                /* To find the dominant orientation, the gradients in x and y are
                   sampled in a circle of radius 6s using wavelets of size 4s.
                   We ensure the gradient wavelet size is even to ensure the 
                   wavelet pattern is balanced and symmetric around its center */
                int grad_wav_size = (int)(2*Math.Round( 2*s ));

                if ( _Height < grad_wav_size || _Width < grad_wav_size )
                {
                    /* when grad_wav_size is too big,
	             * the sampling of gradient will be meaningless
	             * mark keypoint for deletion. */
                    kp.Size = -1;

                    continue;
                }

                icvResizeHaarPattern( dx_s, dx_t, NX, 4, grad_wav_size, _Width);
                icvResizeHaarPattern( dy_s, dy_t, NY, 4, grad_wav_size, _Width);

                for( kk = 0, nangle = 0; kk < nangle0; kk++ )
                {
                    int* ptr;
                    float vx, vy;
                    x = (int)Math.Round( center.X + apt[kk].X*s - (float)(grad_wav_size-1)/2 );
                    y = (int)Math.Round( center.Y + apt[kk].Y*s - (float)(grad_wav_size-1)/2 );

                    if( y >= (_Height - grad_wav_size) ||
                        x >= (_Width - grad_wav_size) )
                        continue;

                    ptr = imgPtr + x + y*sum_cols;
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
                    kp.Size = -1;
                    continue;
                }
                
                _X.Cols = _Y.Cols = _angle.Cols = nangle;
                
                
                //cvCartToPolar( &_X, &_Y, 0, &_angle, 1 );
                Cv.CartToPolar(_X,_Y,null ,_angle, AngleUnit.Degrees );

                float bestx = 0, besty = 0, descriptor_mod = 0;


                for( i = 0; i < 360; i += ORI_SEARCH_INC )
                {
                    float sumx = 0, sumy = 0, temp_mod;
                    for( j = 0; j < nangle; j++ )
                    {
                        int d = (int) Math.Abs( Math.Round(angle[j]) - i);

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
                
                float descriptor_dir = Cv.FastArctan( besty, bestx );
                                

                kp.Dir  = descriptor_dir;

                //if( !_descriptors )
                //    continue;

                descriptor_dir *= (float)( Math.PI/180);
                
                /* Extract a window of pixels around the keypoint of size 20s */
                int win_size = (int)((PATCH_SZ+1)*s);

                //if( win_bufs[thread_idx] == 0 || win_bufs[thread_idx]->cols < win_size*win_size )
                //{
                //    cvReleaseMat( &win_bufs[thread_idx] );
                //    win_bufs[thread_idx] = cvCreateMat( 1, win_size*win_size, CV_8U );
                //}
                
                // 초기화 처리 해야함
                if( win_bufs == null || win_bufs.Cols  < win_size*win_size )
                {
                    //cvReleaseMat( &win_bufs[thread_idx] );
                    
                    win_bufs =  new CvMat( 1, win_size*win_size, MatrixType.U8C1);
                }
                


                CvMat win = new CvMat (win_size, win_size, MatrixType.U8C1,win_bufs.ToArray());
                
                float sin_dir = (float) Math.Sin(descriptor_dir);
                float cos_dir = (float) Math.Cos(descriptor_dir) ;

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
                float start_x = center.X + win_offset*cos_dir + win_offset*sin_dir;
                float start_y = center.Y - win_offset*sin_dir + win_offset*cos_dir;

                byte * WIN = (byte*)win.Data;

                for( i=0; i<win_size; i++, start_x+=sin_dir, start_y+=cos_dir )
                {
                    float pixel_x = start_x;
                    float pixel_y = start_y;
                    for( j=0; j<win_size; j++, pixel_x+=cos_dir, pixel_y-=sin_dir )
                    {
                        x = (int)Math.Round( pixel_x );
                        y = (int)Math.Round( pixel_y );

                        x = Math.Max( x, 0 );
                        y = Math.Max( y, 0 );
                        x = Math.Min( x, _Width-1 );
                        y = Math.Min( y, _Height-1 );

                        WIN[i*win_size + j] = img[y*data.Stride+x];
                     }
                }

                /* Scale the window to size PATCH_SZ so each pixel's size is s. This
                   makes calculating the gradients with wavelets of size 2s easy */

                //cvResize( &win, &_patch, CV_INTER_AREA );
                Cv.Resize(win,_patch, Interpolation.Area);

                /* Calculate gradients in x and y with wavelets of size 2s */
                for( i = 0; i < PATCH_SZ; i++ )
                    for( j = 0; j < PATCH_SZ; j++ )
                    {
                        float dw = DW[i,j];
                        float vx = (PATCH[i,j+1] - PATCH[i,j] + PATCH[i+1,j+1] - PATCH[i+1,j])*dw;
                        float vy = (PATCH[i+1,j] - PATCH[i,j] + PATCH[i+1,j+1] - PATCH[i,j+1])*dw;
                        DX[i,j] = vx;
                        DY[i,j] = vy;
                    }

                /* Construct the descriptor */
                //vec = (float*)cvGetSeqElem( descriptors, k );
                //float* vec =(float*) Cv.SeqPushMulti<float*>( descriptors, k );
                //Cv.getse
                float [] _vec = new float [descriptor_size];

                fixed(float * pVec = _vec)
                {
                    float * vec = pVec;

                //for( kk = 0; kk < descriptor_size; kk++ )
                //    vec[kk] = 0;

                double square_mag = 0;       

                //if( params.extended )
                    if( true )
                    {
                        /* 128-bin descriptor */
                        for( i = 0; i < 4; i++ )
                            for( j = 0; j < 4; j++ )
                            {
                                for( y = i*5; y < i*5+5; y++ )
                                {
                                    for( x = j*5; x < j*5+5; x++ )
                                    {
                                        float tx = DX[y,x], ty = DY[y,x];
                                        if( ty >= 0 )
                                        {
                                            vec[0] += tx;
                                            vec[1] += (float)Math.Abs(tx);
                                        } else {
                                            vec[2] += tx;
                                            vec[3] += (float)Math.Abs(tx);
                                        }
                                        if ( tx >= 0 )
                                        {
                                            vec[4] += ty;
                                            vec[5] += (float)Math.Abs(ty);
                                        } else {
                                            vec[6] += ty;
                                            vec[7] += (float)Math.Abs(ty);
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
                                        float tx = DX[y,x], ty = DY[y,x];
                                        vec[0] += tx; vec[1] += ty;
                                        vec[2] += (float)Math.Abs(tx); vec[3] += (float)Math.Abs(ty);
                                    }
                                }
                                for( kk = 0; kk < 4; kk++ )
                                    square_mag += vec[kk]*vec[kk];
                                vec+=4;
                            }
                    }
                

                vec = pVec;

                /* unit vector is essential for contrast invariance */
                //vec = (float*)cvGetSeqElem( descriptors, k );


                double scale = 1 / (Math.Sqrt(square_mag) + double.Epsilon);
                for( kk = 0; kk < descriptor_size; kk++ )
                    vec[kk] = (float)(vec[kk]*scale);
                }
            }


            }
            return descriptors;
        }
    

        unsafe int icvInterpolateKeypoint( float[,] N9, int dx, int dy, int ds, CvSURFPoint point )
        {

             //CvInvoke.cvso
            int solve_ok;
            float [] A= new float[9];
                float [] x = new float [3];
                    float [] b = new float [3];

            CvMat _A = new CvMat( 3, 3,  MatrixType.F32C1, A);
            CvMat _x = new CvMat(3, 1, MatrixType.F32C1, x);                
            CvMat _b = new CvMat(3, 1, MatrixType.F32C1, b);

            b[0] = -(N9[1,5]-N9[1,3])/2;  /* Negative 1st deriv with respect to x */
            b[1] = -(N9[1,7]-N9[1,1])/2;  /* Negative 1st deriv with respect to y */
            b[2] = -(N9[2,4]-N9[0,4])/2;  /* Negative 1st deriv with respect to s */

            A[0] = N9[1,3]-2*N9[1,4]+N9[1,5];            /* 2nd deriv x, x */
            A[1] = (N9[1,8]-N9[1,6]-N9[1,2]+N9[1,0])/4; /* 2nd deriv x, y */
            A[2] = (N9[2,5]-N9[2,3]-N9[0,5]+N9[0,3])/4; /* 2nd deriv x, s */
            A[3] = A[1];                                    /* 2nd deriv y, x */
            A[4] = N9[1,1]-2*N9[1,4]+N9[1,7];            /* 2nd deriv y, y */
            A[5] = (N9[2,7]-N9[2,1]-N9[0,7]+N9[0,1])/4; /* 2nd deriv y, s */
            A[6] = A[2];                                    /* 2nd deriv s, x */
            A[7] = A[5];                                    /* 2nd deriv s, y */
            A[8] = N9[0,4]-2*N9[1,4]+N9[2,4];            /* 2nd deriv s, s */

            solve_ok = CvInvoke.cvSolve(_A.CvPtr,_b.CvPtr, _x.CvPtr,InvertMethod.LU);

            if( solve_ok != 0)
            {
                point.Pt.X += x[0]*dx;
                point.Pt.Y += x[1]*dy;
                point.Size = (int) Math.Round( point.Size + x[2]*ds ); 
            }
            return solve_ok;
        }
    
        unsafe float icvCalcHaarPattern(int* origin, CvSurfHF[] f, int n)
        {
            double d = 0;
            for( int k = 0; k < n; k++ )
                d += (origin[f[k].p0] + origin[f[k].p3] - origin[f[k].p1] - origin[f[k].p2])*f[k].w;
            return (float)d;

        }



        void icvResizeHaarPattern(int[,] src, CvSurfHF[] dst, int n, int oldSize, int newSize, int widthStep)
        {
            

            float ratio = (float)newSize / oldSize;
            for (int k = 0; k < n; k++)
            {
                int dx1 = (int) Math.Round(ratio * src[k,0]);
                int dy1 = (int) Math.Round(ratio * src[k,1]);
                int dx2 = (int) Math.Round(ratio * src[k,2]);
                int dy2 = (int) Math.Round(ratio * src[k,3]);
                dst[k].p0 = dy1 * widthStep + dx1;
                dst[k].p1 = dy2 * widthStep + dx1;
                dst[k].p2 = dy1 * widthStep + dx2;
                dst[k].p3 = dy2 * widthStep + dx2;
                dst[k].w = src[k,4] / ((float)(dx2 - dx1) * (dy2 - dy1));
            }
        }

        public List<CvSURFPoint> FastHessianDetector(Bitmap ibmp)
        {
            const int nOctaves = 4;
            const int nOctaveLayers = 2;
            const int hessianThreshold = 500;



            const int NX=3, NY=3, NXY=4, NM=1;

            const int HAAR_SIZE0 = 9;
            const int HAAR_SIZE_INC = 6;


            int margin,size;

            float dx, dy, dxy;

            int [,] dx_s = new int[NX,5] { {0, 2, 3, 7, 1}, {3, 2, 6, 7, -2}, {6, 2, 9, 7, 1} };
            int[,] dy_s = new int [NY,5]  { { 2, 0, 7, 3, 1 }, { 2, 3, 7, 6, -2 }, { 2, 6, 7, 9, 1 } };
            int [,] dxy_s = new int [NXY,5]{ {1, 1, 4, 4, 1}, {5, 1, 8, 4, -1}, {1, 5, 4, 8, -1}, {5, 5, 8, 8, 1} };
            int [,] dm = new int [1,5] { {0, 0, 9, 9, 1} };

            CvSurfHF[] Dx = new CvSurfHF[NX] { new CvSurfHF(), new CvSurfHF(), new CvSurfHF() };
            CvSurfHF[] Dy = new CvSurfHF[NY] { new CvSurfHF(), new CvSurfHF(), new CvSurfHF() };
            CvSurfHF[] Dxy = new CvSurfHF[NXY] { new CvSurfHF(), new CvSurfHF(), new CvSurfHF(), new CvSurfHF() };
            CvSurfHF[] Dm = new CvSurfHF[1] { new CvSurfHF() };

            Rectangle rect = new Rectangle(0, 0, ibmp.Width, ibmp.Height);
            System.Drawing.Imaging.BitmapData data = ibmp.LockBits(rect, System.Drawing.Imaging.ImageLockMode.ReadWrite,
                ibmp.PixelFormat);

            integralMap = Integral(data);

            this._Width = data.Width;
            this._Height = data.Height;

            float[][] dets = new float[nOctaves][] ;
            float[][] traces = new float[nOctaves][];
            int[] HaarPatternSizes = new int[nOctaves];

            for (int layer = 0; layer <= nOctaves - 1; layer++)
            {
                dets[layer] = new float[(data.Width-1) *(data.Height-1) ];
                traces[layer] = new float[(data.Width - 1) * (data.Height - 1)];
                
            }
            

            //CvMemStorage storage = Cv.CreateMemStorage();
            //CvSeq keypoints = Cv.CreateSeq(SeqType.Zero, CvSeq.SizeOf, CvSeq<CvSURFPoint>.SizeOf, storage);
            
            List<CvSURFPoint> keypoints = new List<CvSURFPoint>();
            
            unsafe
            {

                //fixed (float* pdets = dets)
                //using (CvMemStorage storage = new CvMemStorage(0))
                fixed (int* imgPtr = integralMap)
                {
                    //keypoints = new CvSeq<CvSURFPoint>(SeqType.Zero, CvSeq < CvSURFPoint >.SizeOf, storage);

                    //byte* imgPtr = (byte*)(data.Scan0);

                    int* s_ptr;
                    float* det_ptr;
                    float* trace_ptr;



                    for (int octave = 0, sampleStep = 1; octave < nOctaves; octave++, sampleStep *= 2)
                    {

                        int rows = (data.Height - 1) / sampleStep;
                        int cols = (data.Width - 1) / sampleStep;


                        for (int layer = 0; layer < nOctaveLayers + 1; layer++)
                        {

                            HaarPatternSizes[layer] = size = (HAAR_SIZE0 + HAAR_SIZE_INC * layer) << octave;

                            icvResizeHaarPattern(dx_s, Dx, NX, 9, size, data.Width);
                            icvResizeHaarPattern(dy_s, Dy, NY, 9, size, data.Width);
                            icvResizeHaarPattern(dxy_s, Dxy, NXY, 9, size, data.Width);

                            margin = (size / 2) / sampleStep;


                            for (int sum_i = 0, i = margin; sum_i <= (data.Height - 1) - size; sum_i += sampleStep, i++)
                            {
                                s_ptr = imgPtr + sum_i * data.Width;

                                fixed (float* pdet_ptr = dets[layer])
                                fixed (float* ptrace_ptr = traces[layer])
                                {



                                    det_ptr = pdet_ptr + i * data.Width + margin;
                                    trace_ptr = ptrace_ptr + i * data.Width + margin;
                                    for (int sum_j = 0, j = margin; sum_j <= (data.Width - 1) - size; sum_j += sampleStep, j++)
                                    {
                                        dx = icvCalcHaarPattern(s_ptr, Dx, 3);
                                        dy = icvCalcHaarPattern(s_ptr, Dy, 3);
                                        dxy = icvCalcHaarPattern(s_ptr, Dxy, 4);
                                        s_ptr += sampleStep;
                                        *det_ptr++ = (float)(dx * dy - 0.81 * dxy * dxy);
                                        *trace_ptr++ = (float)(dx + dy);
                                    }
                                }
                            }


                        }

                        /* Find maxima in the determinant of the hessian */
                        for (int layer = 1; layer <= nOctaveLayers; layer++)
                        {
                            size = HaarPatternSizes[layer];
                            icvResizeHaarPattern(dm, Dm, NM, 9, size, data.Width);

                            /* Ignore pixels without a 3x3 neighbourhood in the layer above */
                            margin = (HaarPatternSizes[layer + 1] / 2) / sampleStep + 1;
                            for (int i = margin; i < rows - margin; i++)
                            {

                                fixed (float* pdet_ptr1 = dets[layer - 1])
                                fixed (float* ptrace_ptr1 = traces[layer - 1])
                                fixed (float* pdet_ptr2 = dets[layer])
                                fixed (float* ptrace_ptr2 = traces[layer])
                                fixed (float* pdet_ptr3 = dets[layer + 1])
                                fixed (float* ptrace_ptr3 = traces[layer + 1])
                                {

                                    det_ptr = pdet_ptr2 + i * data.Width;
                                    trace_ptr = ptrace_ptr2 + i * data.Width;

                                    for (int j = margin; j < cols - margin; j++)
                                    {
                                        float val0 = det_ptr[j];
                                        if (val0 > hessianThreshold)
                                        {
                                            /* Coordinates for the start of the wavelet in the sum image. There   
                                               is some integer division involved, so don't try to simplify this
                                               (cancel out sampleStep) without checking the result is the same */
                                            int sum_i = sampleStep * (i - (size / 2) / sampleStep);
                                            int sum_j = sampleStep * (j - (size / 2) / sampleStep);

                                            /* The 3x3x3 neighbouring samples around the maxima. 
                                               The maxima is included at N9[1][4] */
                                            int c = data.Width;
                                            float* det1 = pdet_ptr1 + i * c + j;
                                            float* det2 = pdet_ptr2 + i * c + j;
                                            float* det3 = pdet_ptr3 + i * c + j;
                                            float[,] N9 = new float[3, 9]{ { det1[-c-1], det1[-c], det1[-c+1],          
                                                                 det1[-1]  , det1[0] , det1[1],
                                                                 det1[c-1] , det1[c] , det1[c+1]  },
                                                               { det2[-c-1], det2[-c], det2[-c+1],       
                                                                 det2[-1]  , det2[0] , det2[1],
                                                                 det2[c-1] , det2[c] , det2[c+1 ] },
                                                               { det3[-c-1], det3[-c], det3[-c+1],       
                                                                 det3[-1  ], det3[0] , det3[1],
                                                                 det3[c-1] , det3[c] , det3[c+1 ] } };

                                            /* Check the mask - why not just check the mask at the center of the wavelet? */
                                            //if( mask_sum )
                                            //{
                                            //    const int* mask_ptr = mask_sum->data.i +  mask_sum->cols*sum_i + sum_j;
                                            //    float mval = icvCalcHaarPattern( mask_ptr, &Dm, 1 );
                                            //    if( mval < 0.5 )
                                            //        continue;
                                            //}

                                            /* Non-maxima suppression. val0 is at N9[1][4]*/
                                            if (val0 > N9[0, 0] && val0 > N9[0, 1] && val0 > N9[0, 2] &&
                                                val0 > N9[0, 3] && val0 > N9[0, 4] && val0 > N9[0, 5] &&
                                                val0 > N9[0, 6] && val0 > N9[0, 7] && val0 > N9[0, 8] &&
                                                val0 > N9[1, 0] && val0 > N9[1, 1] && val0 > N9[1, 2] &&
                                                val0 > N9[1, 3] && val0 > N9[1, 5] &&
                                                val0 > N9[1, 6] && val0 > N9[1, 7] && val0 > N9[1, 8] &&
                                                val0 > N9[2, 0] && val0 > N9[2, 1] && val0 > N9[2, 2] &&
                                                val0 > N9[2, 3] && val0 > N9[2, 4] && val0 > N9[2, 5] &&
                                                val0 > N9[2, 6] && val0 > N9[2, 7] && val0 > N9[2, 8])
                                            {
                                                /* Calculate the wavelet center coordinates for the maxima */
                                                float center_i = sum_i + (float)(size - 1) / 2;
                                                float center_j = sum_j + (float)(size - 1) / 2;

                                                CvSURFPoint point = new CvSURFPoint();

                                                point.Pt.X = center_j;
                                                point.Pt.Y = center_i;

                                                if (0 < trace_ptr[j])
                                                {
                                                    point.Laplacian = 1;
                                                }
                                                else if (trace_ptr[j] < 0)
                                                {
                                                    point.Laplacian = -1;

                                                }
                                                else
                                                {
                                                    point.Laplacian = 0;
                                                }

                                                point.Size = HaarPatternSizes[layer];

                                                point.Dir = 0;
                                                point.Hessian = val0;


                                                /* Interpolate maxima location within the 3x3x3 neighbourhood  */
                                                int ds = HaarPatternSizes[layer] - HaarPatternSizes[layer - 1];
                                                int interp_ok = icvInterpolateKeypoint(N9, sampleStep, sampleStep, ds, point);

                                                /* Sometimes the interpolation step gives a negative size etc. */
                                                if (interp_ok != 0 && point.Size >= 1 &&
                                                    point.Pt.X >= 0 && point.Pt.X <= (data.Width - 1) &&
                                                    point.Pt.Y >= 0 && point.Pt.Y <= (data.Height - 1))
                                                {
                                                    /*printf( "KeyPoint %f %f %d\n", point.pt.x, point.pt.y, point.size );*/

                                                    
                                                    keypoints.Add(point);
                                                    //Cv.SeqPush<CvSURFPoint>(keypoints, point);
                                                }
                                            }
                                        }
                                    }
                                }
                            }

                        }
                    }




                    // Unlock the bits.
                    


                }
            }

            ibmp.UnlockBits(data);

            return keypoints;
                
        }



        unsafe public int[] Integral(System.Drawing.Imaging.BitmapData data) 
        {


            int[] integralMap = new int[data.Width * data.Height];

            //unsafe
            {
                byte* imgPtr = (byte*)(data.Scan0);


                //fixed (int* pIntegralMap = integralMap)
                {

                    integralMap[0] = *imgPtr;

                    for(int x = 1;x < data.Width;x++)
                    {
                        integralMap[x] = integralMap[x-1] + imgPtr[x];
                    }
                    
                    for (int y = 1; y < data.Height; y++)
                    {
                        int linesum = 0;
                                                
                        for (int x = 0; x < data.Width ; x++)
                        {
                            linesum += imgPtr[y * data.Stride + x];
                            integralMap[y * data.Width + x] = integralMap[(y - 1) * data.Width + x] + linesum;

                        }


//                        imgPtr += data.Stride - data.Width + 1;
                    }
                }


            }


            // Unlock the bits.
            //ibmp.UnlockBits(data);



            return integralMap;
        }

    }
}
