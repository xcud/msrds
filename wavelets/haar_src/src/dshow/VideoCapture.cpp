

#include "stdafx.h"
#include "VideoCapture.h"


VideoCapture::VideoCapture() : m_Status(OK), hr(S_OK),
                               m_pMC(NULL), m_pGraph(NULL), m_pCapture(NULL), 
                               m_pGrabber(NULL),
                               m_pFrameData(0) 
{
        m_DeviceName = L"No video device connected.";
        ZeroMemory(&m_BmiHeader, sizeof(BITMAPINFOHEADER));

        hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
        if (FAILED(hr)) {
                m_Status = COMFAILED;
                return;
        }

        // Create the filter graph
        hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC,
                              IID_IGraphBuilder, (void **)&m_pGraph);
        if (FAILED(hr)) {
                m_Status = FGFAILED;
                return;
        }
        // Create the capture graph builder
        hr = CoCreateInstance(CLSID_CaptureGraphBuilder2 , NULL, CLSCTX_INPROC,
                              IID_ICaptureGraphBuilder2, (void **)&m_pCapture);
        if (FAILED(hr)) {
                m_Status = GBFAILED;
                return;
        }
        // Obtain interface for media control
        hr = m_pGraph->QueryInterface(IID_IMediaControl, (LPVOID *)&m_pMC);
        if (FAILED(hr)) {
                m_Status = MCFAILED;
                return;
        }                
}

VideoCapture::~VideoCapture()
{
        if (m_pFrameData != 0) delete[] m_pFrameData;

        Stop();

        SAFE_RELEASE(m_pGrabber);
        SAFE_RELEASE(m_pMC);
        SAFE_RELEASE(m_pGraph);
        SAFE_RELEASE(m_pCapture); 

        if (GetStatus() != COMFAILED)
                CoUninitialize();
}

int VideoCapture::Connect(const wchar_t* deviceName)
{
        if (GetStatus() != OK)
                return -1;

        hr = m_pCapture->SetFiltergraph(m_pGraph);
        if (FAILED(hr)) 
                return -2;

        IBaseFilter* pSrcFilter = NULL;
        hr = FindCaptureDevice(deviceName, &pSrcFilter);
        if (FAILED(hr)) 
                return -3;        

        // Add Capture filter to our graph.
        hr = m_pGraph->AddFilter(pSrcFilter, L"Video Capture");
        if (FAILED(hr)) {
                pSrcFilter->Release();
                return -3;
        }

        //Add Sample Grabber filter
        IBaseFilter* pGrabberFilter = NULL;
        hr = AddSampleGrabber(&pGrabberFilter);
        if (FAILED(hr)) {
                pSrcFilter->Release();
                return -4;
        }

        IBaseFilter* pNullFilter = NULL;
        hr = AddNullRenderer(&pNullFilter);
        if (FAILED(hr)) {
                pGrabberFilter->Release();
                pSrcFilter->Release();
                return -5;
        }        


        // Render the preview pin on the video capture filter        
        hr = m_pCapture->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video,
                                      pSrcFilter, pGrabberFilter, pNullFilter);
        if (FAILED(hr)) {
                pNullFilter->Release();
                pGrabberFilter->Release();
                pSrcFilter->Release();
                return -6;
        }
        
        pNullFilter->Release();
        pGrabberFilter->Release();
        pSrcFilter->Release();

        m_Status = CONNECTED;        

        hr = GetMediaType();
        if (FAILED(hr))                 
                return -7;        
                
        return 0;
}

int VideoCapture::Start()
{
        if (GetStatus() == CONNECTED || GetStatus() == STOPED) {
                hr = m_pMC->Run();
                if (FAILED(hr)) 
                        return -2;
                m_Status = RUNNING;
                return 0;
        }
        else
                return -1;
}

int VideoCapture::Stop()
{
        if (GetStatus() == RUNNING) {
                hr = m_pMC->StopWhenReady();
                if (FAILED(hr))
                        return -2;
                m_Status = STOPED;
                return 0;
        }
        else
                return -1;
}

const BYTE* VideoCapture::GrabFrame()
{
        if (GetStatus() != RUNNING) 
                return 0;

        LONG size = m_BmiHeader.biSizeImage;
        hr = m_pGrabber->GetCurrentBuffer(&size, (LONG *)m_pFrameData);
        if (SUCCEEDED(hr))                        
        {
                FlipFrame();
                return m_pFrameData;
        }
        else
                return 0;
}

void VideoCapture::FlipFrame() const
{     
        unsigned int stride = m_BmiHeader.biWidth * (m_BmiHeader.biBitCount / 8);
        unsigned char* pScan1 = m_pFrameData;
        unsigned char* pScan2 = m_pFrameData + (m_BmiHeader.biSizeImage - stride);
        for (unsigned int y = 0; y < (unsigned int)m_BmiHeader.biHeight / 2; y++) {
                for (unsigned int x = 0; x < stride; x++) 
                        std::swap(pScan1[x], pScan2[x]);                
                pScan1 += stride;
                pScan2 -= stride;
        }
}

HRESULT VideoCapture::FindCaptureDevice(const wchar_t* deviceName, IBaseFilter** ppSrcFilter)
{
        ICreateDevEnum *pSysDevEnum = NULL;
        hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
                              IID_ICreateDevEnum, (void **)&pSysDevEnum);
        if (FAILED(hr)) 
                return hr;        

        // Obtain a class enumerator for the video input devices category.
        IEnumMoniker *pEnumCat = NULL;
        hr = pSysDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnumCat, 0);
        if (hr == S_OK) {
                // Enumerate the monikers.
                IMoniker *pMoniker = NULL;
                ULONG cFetched;
                while (pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK) {
                        IPropertyBag *pPropBag;
                        hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag,
                                                     (void **)&pPropBag);
                        if (SUCCEEDED(hr)) {
                                // To retrieve the filter's friendly name
                                VARIANT varName;
                                VariantInit(&varName);
                                hr = pPropBag->Read(L"FriendlyName", &varName, 0);
                                if (SUCCEEDED(hr)) {
                                        if (wcscmp(deviceName, varName.bstrVal) == 0) {
                                                m_DeviceName = varName.bstrVal;
                                                VariantClear(&varName);    

                                                IBaseFilter* pFilter;
                                                hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter,
                                                                            (void**)&pFilter);
                                                if (SUCCEEDED(hr)) {
                                                        *ppSrcFilter = pFilter;
                                                        (*ppSrcFilter)->AddRef();
                                                        pFilter->Release();
                                                }
                                                
                                                pPropBag->Release();        
                                                pMoniker->Release();
                                                pEnumCat->Release();
                                                pSysDevEnum->Release();
                                                return hr;
                                        }                                        
                                }
                                VariantClear(&varName);                                
                                pPropBag->Release();
                        }
                        pMoniker->Release();
                }
                pEnumCat->Release();
        }
        else {
                pSysDevEnum->Release(); 
                return hr;
        }

        pSysDevEnum->Release();
        return -1;        
}

HRESULT VideoCapture::AddSampleGrabber(IBaseFilter** ppGrabberFilter)
{                
        IBaseFilter* pFilter;
        hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,
                              IID_IBaseFilter, (void **)&pFilter);
        if (FAILED(hr))                 
                return hr;        

        *ppGrabberFilter = pFilter;

        hr = m_pGraph->AddFilter(pFilter, L"Sample Grabber");
        if (FAILED(hr)) {
                pFilter->Release();                
                return hr;
        }

        SAFE_RELEASE(m_pGrabber);
        pFilter->QueryInterface(IID_ISampleGrabber, (void **)&m_pGrabber);        

        AM_MEDIA_TYPE mt;
        ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
        mt.majortype = MEDIATYPE_Video;
        mt.subtype = MEDIASUBTYPE_RGB24;

        hr = m_pGrabber->SetMediaType(&mt);
        if (FAILED(hr)) {        
                m_pGrabber->Release();
                pFilter->Release();
                return hr;
        }

        FreeMediaType(mt);

        hr = m_pGrabber->SetOneShot(FALSE);
        hr = m_pGrabber->SetBufferSamples(TRUE);

        (*ppGrabberFilter)->AddRef();
         pFilter->Release();

        return hr;
}

HRESULT VideoCapture::GetMediaType()
{
        AM_MEDIA_TYPE mt;
        HRESULT hr = m_pGrabber->GetConnectedMediaType(&mt);
        if (FAILED(hr)) {
                FreeMediaType(mt);
                return hr;
        }
        
        memcpy(&m_BmiHeader, &((VIDEOINFOHEADER *)mt.pbFormat)->bmiHeader, sizeof(BITMAPINFOHEADER));
        if (m_pFrameData != 0) delete[] m_pFrameData;
        m_pFrameData = new BYTE[m_BmiHeader.biSizeImage];

        FreeMediaType(mt);
        return hr;
}

void VideoCapture::FreeMediaType(AM_MEDIA_TYPE& mt) const
{
        if (mt.cbFormat != 0) {
                CoTaskMemFree((PVOID)mt.pbFormat);
                mt.cbFormat = 0;
                mt.pbFormat = NULL;
        }
        if (mt.pUnk != NULL) {
                mt.pUnk->Release();
                mt.pUnk = NULL;
        }
}

HRESULT VideoCapture::AddNullRenderer(IBaseFilter** ppNullFilter) const
{
        IBaseFilter* pFilter;
        hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER,
                              IID_IBaseFilter, (void **)&pFilter);        
        if (FAILED(hr))
                return hr;

        *ppNullFilter = pFilter;

        hr = m_pGraph->AddFilter(pFilter, L"Null Renderer");
        if (FAILED(hr)) {
                pFilter->Release();                
                return hr;
        }

        (*ppNullFilter)->AddRef();
         pFilter->Release();

         return hr;
}
