
//CLROpenCV.h

/*

작성자 : 구본수
e-mail : perpet@hitel.net
닷넷과 opencv 를 연동하는 방식을 테스트함..
최적화는 전혀 안되여있고 단지 어떤식으로 연동할지만 테스트한것입니다.
원래는 들어오는 이미지를 그레이 처리 하려 했지만 opencv 를 전혀 몰라 그냥 이미지 만드는 함수만 사용해봤습니다.

테스트 방법

우선 opencv과깔려 있어야합니다. 버전은 1.0입니다.
opencv 관련 dll을 모두 C:\Microsoft Robotics Studio (1.5)\bin
에 깔아주시고요.

소스의 WebCam 을 컴파일합니다.

그후 vpl 툴에서 WebCam 아이콘을 끌어다 놓고 실행합니다.

웹으로 웹켐을 보시면 이미지가 반으로 짤려 나올것입니다.

모듈 디버깅을 편하게 하시려면 vpl 로 작업된것을 build->compile as a service 로 프로젝트를 빌드하신후

뽑아져 나온 소스와 해당모듈을 같이 솔류선안에 묶어서 작업하시면 직접 해당위치 브레이크 걸면서 쉽게 디버깅하실수있습니다..


*/


#pragma once

using namespace System;
using namespace System::Drawing;
using namespace System::Drawing::Imaging;

namespace CLROpenCVs {


	public ref class ImageInfo
	{
	public:
		int _facePosX; // -1,0,1
		int _facePosY; // -1,0,1
		int _faceCount;
	};


	public ref class CLROpenCV
	{

		IplImage * _pColor;
		IplImage * _pGray;
		int _channels;

		

		CvHaarClassifierCascade* _cascade;

		int _width;
		int _height;

		CvMemStorage* _storage;// = cvCreateMemStorage(0);

		


		// 닷넷 비트맵을 opencv 이미지로 복사
		bool BitmapToCVImage(Drawing::Bitmap ^ pBitMap,IplImage * pImage)
		{
			//ImageFormat ^ imageFormat = pImage->RawFormat->Bmp;

			BYTE * pDest = (BYTE*)pImage->imageData;

			int ws = pImage->widthStep;

			//int w = pImage->Width;

			System::Drawing::Rectangle ^ rectancle = System::Drawing::Rectangle( 0 , 0 , pBitMap->Width , pBitMap->Height ) ;

			BitmapData ^ data = pBitMap->LockBits( *rectancle,
				ImageLockMode::ReadWrite  , PixelFormat::Format24bppRgb  );


			BYTE * pSrc = (BYTE*) data->Scan0.ToPointer(); 


			for( int h = 0 ; h < pBitMap->Height ; h++ )
				for( int w = 0 ; w < pBitMap->Width ; w++ )
				{
					pDest[h*ws + w*3    ] = pSrc[h*data->Stride + w*3	  ];
					pDest[h*ws + w*3 + 1] = pSrc[h*data->Stride + w*3  + 1];
					pDest[h*ws + w*3 + 2] = pSrc[h*data->Stride + w*3  + 2];
				}

				pBitMap->UnlockBits(data);

				return true;
		}
	public:

			

		// opencv 이미지를 닷넷 비트맵으로 복사
		static bool CVImageToBitmap_Gray(IplImage * pImage, Drawing::Bitmap ^ pBitMap)
		{
			//ImageFormat ^ imageFormat = pImage->RawFormat->Bmp;

			BYTE * pSrc = (BYTE*)pImage->imageData;

			int ws = pImage->widthStep;

			//int w = pImage->Width;

			System::Drawing::Rectangle ^ rectancle = System::Drawing::Rectangle( 0 , 0 , pBitMap->Width , pBitMap->Height ) ;

			BitmapData ^ data = pBitMap->LockBits( *rectancle,
				ImageLockMode::ReadWrite  , PixelFormat::Format8bppIndexed  );


			BYTE * pDest = (BYTE*) data->Scan0.ToPointer(); 

			for( int h = 0 ; h < pImage->height  ; h++ )
			{
				memcpy(pDest + (pImage->height - h -1) * pBitMap->Width, pSrc + h * ws ,pImage->width);
			}

			pBitMap->UnlockBits(data);

			return true;
		}

		// opencv 이미지를 닷넷 비트맵으로 복사
		static bool CVImageToBitmap(IplImage * pImage, Drawing::Bitmap ^ pBitMap)
		{
			//ImageFormat ^ imageFormat = pImage->RawFormat->Bmp;

			BYTE * pDest = (BYTE*)pImage->imageData;

			int ws = pImage->widthStep;

			//int w = pImage->Width;

			System::Drawing::Rectangle ^ rectancle = System::Drawing::Rectangle( 0 , 0 , pBitMap->Width , pBitMap->Height ) ;

			BitmapData ^ data = pBitMap->LockBits( *rectancle,
				ImageLockMode::ReadWrite  , PixelFormat::Format24bppRgb  );


			BYTE * pSrc = (BYTE*) data->Scan0.ToPointer(); 

			for( int h = 0 ; h < pBitMap->Height  ; h++ )
				for( int w = 0 ; w < pBitMap->Width ; w++ )
				{
					pSrc[h*data->Stride + w*3	  ] = pDest[h*ws + w*3    ];
					pSrc[h*data->Stride + w*3  + 1] = pDest[h*ws + w*3 + 1];
					pSrc[h*data->Stride + w*3  + 2] = pDest[h*ws + w*3 + 2];
				}

				pBitMap->UnlockBits(data);

				return true;
		}

		int detectObjects(IplImage* image)
		{
			
			CvSeq* faces;
			int i, scale = 1;

			/* use the fastest variant */
			faces = cvHaarDetectObjects( image, _cascade, _storage, 1.2, 2, CV_HAAR_DO_CANNY_PRUNING );

			/* draw all the rectangles */
			for( i = 0; i < faces->total; i++ )
			{
				/* extract the rectanlges only */
				CvRect face_rect = *(CvRect*)cvGetSeqElem( faces, i);//, 0 );
				cvRectangle( image, cvPoint(face_rect.x*scale,face_rect.y*scale),
					cvPoint((face_rect.x+face_rect.width)*scale,
					(face_rect.y+face_rect.height)*scale),
					CV_RGB(0,0,255), 3 );
				int center_x = (face_rect.x+face_rect.width/2) ;
				if( center_x < _width/3)
				{
					_pImageInfo->_facePosX = -1;
				}else if( center_x < (_width*2/3))
				{
					_pImageInfo->_facePosX = 0;
				}else
				{
					_pImageInfo->_facePosX = 1;
				}
				
				int center_y = (face_rect.y+face_rect.height/2) ;
				if( center_y < _height/3)
				{
					_pImageInfo->_facePosY = 1;
				}else if( center_y < (_height*2/3))
				{
					_pImageInfo->_facePosY = 0;
				}else
				{
					_pImageInfo->_facePosY = -1;
				}

			}

			return 0;			
		}



	public :

		ImageInfo ^ _pImageInfo;


		CLROpenCV()
		{
			_pColor = NULL;
			_pGray = NULL;
			_pImageInfo = gcnew ImageInfo();
			_storage = cvCreateMemStorage(0);
		}
		
		~CLROpenCV()
		{
			
			if(_pColor)
			{
				pin_ptr<IplImage*> p = &_pColor;
				cvReleaseImage(p);
			}
			if(_pGray )
			{
				pin_ptr<IplImage*> p = &_pGray;
				cvReleaseImage(p);
			}
			{
				pin_ptr<CvMemStorage*> p = &_storage;
				cvReleaseMemStorage( p );	
			}

			{
				pin_ptr<CvHaarClassifierCascade* > p = &_cascade;
				cvReleaseHaarClassifierCascade( p );	
			}


			
			
		}



		bool Init(int w,int h,int bitCount)
		{
			_width = w;
			_height = h;


			if( bitCount == 8)
			{
				_channels = 1;
			}else
			{
				_channels = 3;
			}
			
			_pColor = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, _channels);
			_pGray = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, _channels);

			
			_cascade = (CvHaarClassifierCascade*)cvLoad("C:\\Microsoft Robotics Studio (1.5)\\bin\\haarcascade_frontalface_default.xml");
			if( !_cascade )
			{
				return false;
			}

			return true;
		}
		
		void Release()
		{
			

		}
		


		bool colorToGray(Drawing::Bitmap ^ pImage)
		{
			BitmapToCVImage(pImage, _pColor);

//			cvCvtColor(_pColor, _pGray, CV_RGB2GRAY,);	// 컬러를 흑백으로 변환 
			
			CVImageToBitmap(_pColor, pImage );

			return true;

		}
		// TODO: 여기에 이 클래스에 대한 메서드를 추가합니다.

		
		int detectFace(Drawing::Bitmap ^ pImage)
		{
			
			BitmapToCVImage(pImage, _pColor);
			
			int ret = detectObjects(_pColor);
			//			cvCvtColor(_pColor, _pGray, CV_RGB2GRAY,);	// 컬러를 흑백으로 변환 

			CVImageToBitmap(_pColor, pImage );

			return ret;

		}

	};
}
