// surfCLR.h

#pragma once

#pragma unmanaged
#include <cv.h>
#include <highgui.h>
#pragma managed

using namespace System;
using namespace System::Drawing;
using namespace System::Drawing::Imaging;

#include <msclr/marshal_cppstd.h>

namespace surfCLR {

	public ref class SURF
	{
		// TODO: Add your methods for this class here.
	private:



		cv::SURF2 * _NativeSURF2;
		CvCapture* _capture;

		
	public: 

				SURF()
		{
		
			_capture = NULL;
		}

		~SURF()
		{
			//cvReleaseCapture( &_capture );
		}

		double GetExtremaTime(){return _NativeSURF2->_extremaTime;}
		double GetDescriptorTime(){return _NativeSURF2->_descriptorTime;}
		double GetCompareTime(){return _NativeSURF2->_compareTime;}

		int GetCX(){return _NativeSURF2->_CX;}
		int GetCY(){return _NativeSURF2->_CY;}
		//double GetExtremaTime(){return _NativeSURF2->_extremaTime;}


		Drawing::Bitmap^ imgToBitmap(IplImage* img)
		{
		
				PixelFormat pf;
				switch(img->nChannels)
				{
                case 1:
					{
					pf = PixelFormat::Format8bppIndexed; 

					//dst.Palette = plt;
			

					Drawing::Bitmap^ bmp = gcnew Drawing::Bitmap(img->width,img->height,img->widthStep, pf,IntPtr( img->imageData));
					//bmp->Palette = plt;

					   ColorPalette^ plt = bmp->Palette;
					for (int x = 0; x < 256; x++)
					{
						plt->Entries[x] = Color::FromArgb(x, x, x);
					}
					
					bmp->Palette = plt;

					return bmp;
					}
					
					
                case 3:
                    pf = PixelFormat::Format24bppRgb; 
					return gcnew Drawing::Bitmap(img->width,img->height,img->widthStep, pf,IntPtr( img->imageData));
					
                case 4:
                    pf = PixelFormat::Format32bppArgb; 
					return gcnew Drawing::Bitmap(img->width,img->height,img->widthStep, pf,IntPtr( img->imageData));
					
                default:
					break;

				}

			return nullptr;		
		}

		Drawing::Bitmap^ Init(String ^ fileName)
		{
			msclr::interop::marshal_context c;

			_capture = cvCaptureFromCAM(0); // capture from video device #0

			
			
			_NativeSURF2 = new cv::SURF2();
			IplImage* object = cvLoadImage(c.marshal_as<const char *>(fileName), CV_LOAD_IMAGE_GRAYSCALE );
//			_NativeSURF2->InitResource();
			_NativeSURF2->Init(object);

			
			return imgToBitmap( object);

		}


		Drawing::Bitmap ^ _bmp ;

		Drawing::Bitmap ^ UpdateFrame()
		{
			IplImage* img = 0; 


			if(!cvGrabFrame(_capture)){              // capture a frame 
			  
				return nullptr;
			  
			}

			img=cvRetrieveFrame(_capture);           // retrieve the captured frame

			_NativeSURF2->Compare(img);

			if(_bmp == nullptr)
			{

				_bmp = imgToBitmap(img);
			}else
			{
				
				BitmapData^ bd = _bmp->LockBits(System::Drawing::Rectangle(0, 0,_bmp->Width, _bmp->Height), ImageLockMode::WriteOnly, _bmp->PixelFormat);
				
				if( bd->Stride == img->widthStep)
					memcpy_s(bd->Scan0.ToPointer(),bd->Stride * bd->Height, img->imageData,img->widthStep * img->height);

				_bmp->UnlockBits(bd);

			

			}


			return _bmp;
		}


	};
}
