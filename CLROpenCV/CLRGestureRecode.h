#pragma once

#include "CLROpenCV.h"
#include "CLROpticalFlow.h"
//#include "CLRGestureDetect.h"



namespace CLROpenCVs {


	
	 public ref class DataInfo
	 {
	 public:
		 System::String ^ _name;
		IplImage * _pImage;
		float _compareResult;	

		DataInfo()
		{
			_compareResult = 0.0f;
		}
			
		virtual System::String ^ ToString() override
		{
			return System::String::Format("{0}({1})",_name->ToString(),_compareResult);
		}

		
	 };

	
	public ref class Option
	{
	public:
		int _gridCount; // 그리드 사이즈
		
	};


	 public ref class CLRGestureRecode // : public CLRGestureDetect
	{
	public:


		// 옵션에 따라 이미지 정보를 조절함..

		int _gridCount; // 그리드 사이즈
		

//typedef std::vector<IplImage*> MODEL_LIST;
		System::Collections::Generic::Dictionary<System::String ^ ,DataInfo ^> _ModelList;

		//MODEL_LIST _ModelList;

		CLRGestureRecode( int gridCount)
		{
			_gridCount = gridCount;
//			_ModelList = new MODEL_LIST;

		};

		~CLRGestureRecode()
		{
			for each(DataInfo ^ pDataInfo in _ModelList.Values)
			{
				//pDataInfo->
				pin_ptr<IplImage*> p = &pDataInfo->_pImage;
				cvReleaseImage( p);
				//cvDestroyImage	()
			}

//			delete _ModelList;
		}

		IplImage * AddModel(System::String ^ name, IplImage* pImage)
		{
			DataInfo  ^ pDataInfo = gcnew DataInfo;
			pDataInfo->_name = name;
			pDataInfo->_pImage = NULL;

			pin_ptr<IplImage*> p = &pDataInfo->_pImage;

			allocateOnDemand(p, pImage->width/_gridCount , pImage->height / _gridCount, IPL_DEPTH_8U, 1 );
			
			cvResize( pImage, pDataInfo->_pImage);
//			pDataInfo->_pImage = cvCloneImage(pImage);

			_ModelList.Add( name,pDataInfo);

			return pDataInfo->_pImage;
		}

		DataInfo ^ GetModel(System::String ^ name)
		{
			return _ModelList[name];
		}

		//8 방향이면 

		/*

		[00][01][02][03]
		[04][05][06][07]
		[08][09][10][11]
		[12][13][14][15]

		*/
		int		GetMovingIndex(IplImage* pOriginImaged,int  gridCount)
		{

			
			int index = -1;

			int indexCount = 0;

			BYTE * pData = (BYTE*)pOriginImaged->imageData;

			int gh = pOriginImaged->height/gridCount;
			int gw = pOriginImaged->width/gridCount;

			int segx;
			int segy;

			int highest  = gh * gw /3;

			// 이미지가 뒤집혀있다

			for(int h = gridCount - 1;0 <= h; h--)
				for(int w = 0;w < gridCount ; w++)
				{
					
					int count =  NoiseCount(pData + (h*gh)*pOriginImaged->widthStep + w*gw,pOriginImaged->widthStep,gw,gh);

					if( highest < count )
					{
						segx = w*gw;
						segy = h*gh;

						highest = count;
						index = indexCount;
					}

					indexCount++;
				}


			if( index == -1)
				return index;
			
			
			
			
			cvRectangle( pOriginImaged, cvPoint( segx,segy), cvPoint(segx+gw,segy+gh), cvScalar(255), 3, 8, 0 ); 

			


			return index;

		}

		

		DataInfo ^ CompareModel(IplImage* pOriginImage , float Threshold)
		{

			DataInfo ^ pBestDataInfo = nullptr;

			IplImage * pImage = NULL;
			pin_ptr<IplImage*> p = &pImage;
			allocateOnDemand(p, pImage->width/_gridCount , pImage->height / _gridCount, IPL_DEPTH_8U, 1 );

			for each(DataInfo ^ pDataInfo in _ModelList.Values)
			{
				pDataInfo->_compareResult = Compare(pImage,pDataInfo->_pImage);

				if( Threshold < pDataInfo->_compareResult)
				{
					if(pBestDataInfo != nullptr && pBestDataInfo->_compareResult < pDataInfo->_compareResult)
					{
						pBestDataInfo = pDataInfo;
					}else 
					{
						pBestDataInfo = pDataInfo;
					}
				}
			}
			
			return pBestDataInfo;
		}
		
	private:

		int NoiseCount(Byte * pData , int ws,int  gw,int gh)
		{
			int count = 0;
			for(int y = 0 ; y < gh; y++)
			{
				for(int x = 0; x < gw ; x++)
				{
					if( 5 < *(pData+x ) )
					{
						++count;
					}

					//*(pData+x ) = 200;

				}
				pData += ws;

			}

			return count;

		}

		float Compare(IplImage * pImage1, IplImage * pImage2)
		{
			BYTE * pBI1 = (BYTE*)pImage1->imageData;
			BYTE * pBI2 = (BYTE*)pImage2->imageData;

			int ws = pImage1->widthStep;

			// 우선 간단하게 하기위해 여분도 대충 검사
			float totalSize = pImage1->height * ws;
			float matchCount = 0;
			for( int h = 0 ; h < pImage1->height  ; h++ )
				for( int w = 0 ; w < ws ; w++ )
				{
					if(pBI1++ || pBI2++)
					{
						matchCount++;	
					}
				}

				

			return matchCount/totalSize;
			
		}

		//virtual IplImage* NextGestureInfo(double frame) new
		//{
		//	CLRGestureDetect::NextGestureInfo( frame);
		//	return NULL;
		//}

		// 무조건 파일부터 오픈

	};
}

