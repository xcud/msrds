#pragma once

#include "CLROpenCV.h"
#include "CLROpticalFlow.h"

namespace CLROpenCVs {





	public ref class CLRGestureDetect
	{
	public:

		IplImage* _OriginaImage;
		CvCapture * _Input_video;
		IplImage* _CompareImage;
		IplImage* _TemporaryImage;
		IplImage* _TemporaryImage2;

		
		IplImage* _DetectImage;


		CvSize*  _size;
		
		
		int _intervalTick;
		int _curTick;

		double _cur_frame;
		double _total_frame;
		
		CLROpticalFlow  ^ _OpticalFlow ;

		int _type ;// 0 : cam , 1 : avi

		CLRGestureDetect()
		{
			_curTick = 0;
			_cur_frame	= 0;
			_OpticalFlow = gcnew CLROpticalFlow;



			_intervalTick = 0;

			_Input_video = NULL;
		};

		~CLRGestureDetect()
		{
		}

		void SetIntervalTick(int intervalTick)
		{
			_intervalTick = intervalTick;
		}
		
		bool InitVideo()
		{

			pin_ptr<IplImage*> p = &_CompareImage;
			allocateOnDemand(p, *_size, IPL_DEPTH_8U, 1 );
			
			p = &_OriginaImage;
			allocateOnDemand(p, *_size, IPL_DEPTH_8U, 1 );

			p = &_TemporaryImage;
			allocateOnDemand(p, *_size, IPL_DEPTH_8U, 1 );
			p = &_TemporaryImage2;
			allocateOnDemand(p, *_size, IPL_DEPTH_8U, 1 );

			
			p = &_DetectImage;
			allocateOnDemand(p, *_size, IPL_DEPTH_8U, 1 );


			cvSetCaptureProperty( _Input_video,	CV_CAP_PROP_POS_FRAMES, 0 );

			IplImage*frame = cvQueryFrame(_Input_video);

			cvConvertImage(frame,_OriginaImage,CV_BGR2GRAY);
			
			//_OpticalFlow->Init(frame,_size);

			return true;
		}

		bool OpenCam(int index )
		{

			if( _Input_video != NULL)
			{
				pin_ptr<CvCapture*> p = &_Input_video;
				cvReleaseCapture(p);
			}

			_Input_video = cvCaptureFromCAM(index);


						_size = new CvSize;
			_size->height = 240;
				//(int) cvGetCaptureProperty( _Input_video, CV_CAP_PROP_FRAME_HEIGHT );
			_size->width = 320;
				//(int) cvGetCaptureProperty( _Input_video, CV_CAP_PROP_FRAME_WIDTH );

			_total_frame = cvGetCaptureProperty( _Input_video, CV_CAP_PROP_FRAME_COUNT );


			_type = 0;

			
			if(!_Input_video)
				return false;

			return InitVideo();
		}

		// 무조건 파일부터 오픈
		bool OpenAviFile(char * fileName)
		{
			if( _Input_video != NULL)
			{
				pin_ptr<CvCapture*> p = &_Input_video;
				cvReleaseCapture(p);
			}

			_Input_video = cvCaptureFromFile(fileName);
			
			if(!_Input_video)
				return false;

			_size = new CvSize;
			_size->height =
				(int) cvGetCaptureProperty( _Input_video, CV_CAP_PROP_FRAME_HEIGHT );
			_size->width =
				(int) cvGetCaptureProperty( _Input_video, CV_CAP_PROP_FRAME_WIDTH );

			_total_frame = cvGetCaptureProperty( _Input_video, CV_CAP_PROP_FRAME_COUNT );


			_type = 1;

			return InitVideo();

		}
		





		// 프레임 값으로 다음 단위를 오픈
		IplImage* NextFrame(double frame)
		{



			if( _type == 0)
			{
				cvGrabFrame(_Input_video);
				IplImage * frame=cvRetrieveFrame(_Input_video);
				
				cvFlip(frame,_TemporaryImage);
		
			}else
			{
				if( _total_frame < frame)
					return NULL;
				cvSetCaptureProperty( _Input_video,	CV_CAP_PROP_POS_FRAMES, frame );
				cvConvertImage(cvQueryFrame(_Input_video),_TemporaryImage,CV_BGR2GRAY);
			}

			return _TemporaryImage;
		}


		IplImage * SetOriginalImage(IplImage * pImage)
		{
			IplImage * temp = _OriginaImage;
			_OriginaImage = pImage;


			return temp;		
		}
		
		
		IplImage* GetOpticalFlowImage()
		{
			return _OpticalFlow->_frame2_1C;
		}

		// 입력된 프레임을 
		virtual IplImage* NextGestureInfo(double frame)
		{
			//_TemporaryImage = NextFrame(frame);

			
			IplImage * curFrame;

			if( _type == 0)
			{
				cvGrabFrame(_Input_video);
				curFrame=cvRetrieveFrame(_Input_video);
				
				//cvFlip(curFrame,_TemporaryImage);

			}else
			{
			if( _total_frame <= frame)
				return NULL;


				cvSetCaptureProperty( _Input_video,	CV_CAP_PROP_POS_FRAMES, frame );
				curFrame = cvQueryFrame(_Input_video);
			}
			
			cvConvertImage(curFrame,_TemporaryImage,CV_BGR2GRAY);

			
			//_OpticalFlow->Process(curFrame);

			
			// 전프레임과 현재프레임의 차를 구한다
			cvAbsDiff(_TemporaryImage,_OriginaImage,_TemporaryImage2);
			
			// 스왑
			//swap(_OriginaImage,_TemporaryImage);
			_TemporaryImage = SetOriginalImage(_TemporaryImage);

			cvThreshold(_TemporaryImage2,_TemporaryImage,10,10,CV_THRESH_BINARY);
			
			if( _intervalTick != 0  && (_curTick + _intervalTick) < System::Environment::TickCount)
			{
				_curTick = System::Environment::TickCount;
				
				ClearImage(_DetectImage,0);
				//cvThreshold(_DetectImage,_DetectImage,0,0,CV_THRESH_BINARY);
			}
			
			cvAdd(_DetectImage,_TemporaryImage,_DetectImage);
			
			
			cvSubS( _DetectImage, cvScalar(5), _DetectImage);
			
			
			
			
			return _TemporaryImage;		
		}
		
		IplImage* GesDetectedInfo()
		{
			return _DetectImage; 
		}

		

	};
}