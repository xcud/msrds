//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;
//using System.Drawing;


//namespace OpenCVForm
//{
//    class Harris
//    {


        

////소스 코드

////4.2 코너 검출
////4.2.1 해리스 코너 검출 함수 선언
////코너점 저장을 위한 구조체 정의
//    const int MAX_CORNER = 5000; //찾을 코너점의 최대 개수
//    public  class CornerPoints
//    {
//     int num;  //현재 저장된 점의 개수
//     int [] x = new int[MAX_CORNER]; //실제 코너점의 x 좌표
//     int [] y = new int[MAX_CORNER]; //실제 코너점의 y 좌표
//    }


    
//    CornerPoints DibHarrisCorner(Bitmap bmp)
//    {
//        int i, j, x, y;
//            Rectangle rect = new Rectangle(0, 0, bmp.Width, bmp.Height);
//            System.Drawing.Imaging.BitmapData data = bmp.LockBits(rect, System.Drawing.Imaging.ImageLockMode.ReadWrite,
//                bmp.PixelFormat);

//         double threshold = 20000.0; //코너 여부를 결정할때 사용하는 코너 응답 함수의 임계값 

//        int w = bmp.Width;
//        int h = bmp.Height;
//                 unsafe
//        {
//               byte* ptr = (byte*)(data.Scan0);
                                    

// //-------------------------------------------------------------------------
// // (fx)*(fx), (fx)*(fy), (fy)*(fy) 계산
// //-------------------------------------------------------------------------

//             double** dx2 = new double*[h];
//             double** dy2 = new double*[h];
//             double** dxy = new double*[h];

// for( i = 0 ; i < h ; i++ )
// {
//  dx2[i] = new double[w];
//  dy2[i] = new double[w];
//  dxy[i] = new double[w];
//  memset(dx2[i], 0, sizeof(int)*w);
//  memset(dy2[i], 0, sizeof(int)*w);
//  memset(dxy[i], 0, sizeof(int)*w);
// }

// double tx, ty;
// for( j = 1 ; j < h-1 ; j++ )
// for( i = 1 ; i < w-1 ; i++ )
// {
//  tx = (ptr[j-1][i+1] + ptr[j][i+1] + ptr[j+1][i+1] 
//   - ptr[j-1][i-1] - ptr[j][i-1] - ptr[j+1][i-1]) / 6.0;
//  ty = (ptr[j+1][i-1] + ptr[j+1][i] + ptr[j+1][i+1] 
//   - ptr[j-1][i-1] - ptr[j-1][i] - ptr[j-1][i+1]) / 6.0;

//  dx2[j][i] = tx * tx;
//  dy2[j][i] = ty * ty;
//  dxy[j][i] = tx * ty;
// }

// //-------------------------------------------------------------------------
// // 가우시안 필터링
// //-------------------------------------------------------------------------

// double** gdx2 = new double*[h];
// double** gdy2 = new double*[h];
// double** gdxy = new double*[h];

// for( i = 0 ; i < h ; i++ )
// {
//  gdx2[i] = new double[w];
//  gdy2[i] = new double[w];
//  gdxy[i] = new double[w];
//  memset(gdx2[i], 0, sizeof(double)*w);
//  memset(gdy2[i], 0, sizeof(double)*w);
//  memset(gdxy[i], 0, sizeof(double)*w);
// }

// double g[5][5] = { {1, 4, 6, 4, 1}, {4, 16, 24, 16, 24}, 
//  {6, 24, 36, 24, 6}, {4, 16, 24, 16, 24}, {1, 4, 6, 4, 1} };

// for( y = 0 ; y < 5 ; y++ )
// for( x = 0 ; x < 5 ; x++ )
// {
//  g[y][x] /= 256.;
// }

// double tx2, ty2, txy;
// for( j = 2 ; j < h-2 ; j++ )
// for( i = 2 ; i < w-2 ; i++ )
// {
//  tx2 = ty2 = txy = 0;
//  for( y = 0 ; y < 5 ; y++ )
//  for( x = 0 ; x < 5 ; x++ )
//  {
//   tx2 += ( dx2[j+y-2][i+x-2] * g[y][x] );
//   ty2 += ( dy2[j+y-2][i+x-2] * g[y][x] );
//   txy += ( dxy[j+y-2][i+x-2] * g[y][x] );
//  }

//  gdx2[j][i] = tx2;
//  gdy2[j][i] = ty2;
//  gdxy[j][i] = txy;
// }

// //-------------------------------------------------------------------------
// // 코너 응답 함수 생성
// //-------------------------------------------------------------------------

// double** crf = new double*[h];
// for( i = 0 ; i < h ; i++ )
// {
//  crf[i] = new double[w];
//  memset(crf[i], 0, sizeof(double)*w);
// }

// double k = 0.04;
// for( j = 2 ; j < h-2 ; j++ )
// for( i = 2 ; i < w-2 ; i++ )
// {
//  crf[j][i] = (gdx2[j][i]*gdy2[j][i] - gdxy[j][i]*gdxy[j][i])
//   - k*(gdx2[j][i] + gdy2[j][i])*(gdx2[j][i] + gdy2[j][i]);
// }

// //-------------------------------------------------------------------------
// // 임계값보다 큰 국지적 최대값을 찾아 코너 포인트로 결정
// //-------------------------------------------------------------------------

// CornerPoints cp;
// cp.num = 0;

// for( j = 2 ; j < h-2 ; j++ )
// for( i = 2 ; i < w-2 ; i++ )
// {
//  if( crf[j][i] > threshold )
//  {
//   if( crf[j][i] > crf[j-1][i] && crf[j][i] > crf[j-1][i+1] &&
//    crf[j][i] > crf[j][i+1] && crf[j][i] > crf[j+1][i+1] &&
//    crf[j][i] > crf[j+1][i] && crf[j][i] > crf[j+1][i-1] &&
//    crf[j][i] > crf[j][i-1] && crf[j][i] > crf[j-1][i-1] )
//   {
//    if( cp.num < MAX_CORNER )
//    {
//     cp.x[cp.num] = i;
//     cp.y[cp.num] = j;
//     cp.num++;
//    }
//   }
//  }
// }

// //-------------------------------------------------------------------------
// // 동적 할당한 메모리 해제
// //-------------------------------------------------------------------------

// for( i = 0 ; i < h ; i++ )
// {
//  delete [] dx2[i];
//  delete [] dy2[i];
//  delete [] dxy[i];
//  delete [] gdx2[i];
//  delete [] gdy2[i];
//  delete [] gdxy[i];
//  delete [] crf[i];
// }

// delete [] dx2;
// delete [] dy2;
// delete [] dxy;
// delete [] gdx2;
// delete [] gdy2;
// delete [] gdxy;
// delete [] crf;

// return cp;
//}

 




//        void Do(Bitmap bmp)
//        {

//            Rectangle rect = new Rectangle(0, 0, bmp.Width, bmp.Height);
//            System.Drawing.Imaging.BitmapData data = bmp.LockBits(rect, System.Drawing.Imaging.ImageLockMode.ReadWrite,
//                bmp.PixelFormat);


//            unsafe
//            {
//                byte* imgPtr = (byte*)(data.Scan0);
//                for (int i = 0; i < data.Height; i++)
//                {
//                    for (int j = 0; j < data.Width / 2; j++)
//                    {
//                        // write the logic implementation here


//                        *imgPtr = 255;

//                        imgPtr++;
//                    }
//                    imgPtr += data.Stride - data.Width;
//                }
//            }


//            // Unlock the bits.
//            bmp.UnlockBits(data);

//            // Draw the modified image.

//        }


//    }
//}
