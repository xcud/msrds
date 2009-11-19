

#ifndef VideoCapture_h
#define VideoCapture_h


#define SAFE_RELEASE(x) { if (x) x->Release(); x = NULL; }


//Video capture -> Sample Grabber -> Null renderer
class VideoCapture
{
public:
        VideoCapture();
        //VideoCapture(const VideoCapture& vc);
        ~VideoCapture();

        enum STATUS { MCFAILED = -4,            //Media control error in constructor
                      GBFAILED = -3,            //Graph builder error in constructor
                      FGFAILED = -2,            //Filter graph error in constructor
                      COMFAILED = -1,           //COM error in constructor
                      OK = 0,                   //constructor OK
                      CONNECTED = 1,            //device is connected and ready to start
                      RUNNING = 2,              //device is capturing data
                      STOPED = 3 };             //device stoped capturing data

// Operators
        //const VideoCapture& operator=(const VideoCapture& vc);

// Operations                
        int Connect(const wchar_t* deviceName);         //connect video device from available devices in the system                
        int Start();                                    //start capturing video data
        int Stop();                                     //stop capturing video data
        const BYTE* GrabFrame();                        //grab frame, returns 0 - ERR device not ready

// Access
        inline enum STATUS GetStatus() const;                                   //video capture device status
        inline HRESULT GetError() const;                                        //get recent HRESULT code        
        inline const std::wstring& GetDeviceName() const;                       //get connected device for video capture
        inline const BITMAPINFOHEADER& GetBitmapInfoHeader() const;             //get connected device video stream params

// Inquiry

protected:
private:
        VideoCapture(const VideoCapture& vc);
        const VideoCapture& operator=(const VideoCapture& vc);

        enum STATUS m_Status;
        mutable HRESULT hr;
        
        IMediaControl* m_pMC;        
        IGraphBuilder* m_pGraph;
        ICaptureGraphBuilder2* m_pCapture;        
        ISampleGrabber* m_pGrabber;

        BITMAPINFOHEADER m_BmiHeader;                   //video stream params
        BYTE* m_pFrameData;                             //last grabbed frame

        std::wstring m_DeviceName;                      //connected device name        


        HRESULT FindCaptureDevice(const wchar_t* deviceName, IBaseFilter** ppSrcFilter);
        HRESULT AddSampleGrabber(IBaseFilter** pGrabberFilter);
        HRESULT GetMediaType();
        void FreeMediaType(AM_MEDIA_TYPE& mt) const;        
        void FlipFrame() const;
        HRESULT AddNullRenderer(IBaseFilter** ppNullFilter) const;
        

};

// Inlines
inline enum VideoCapture::STATUS VideoCapture::GetStatus() const
{
        return m_Status;
}

inline HRESULT VideoCapture::GetError() const
{
        return hr;
}

inline const std::wstring& VideoCapture::GetDeviceName() const
{
        return m_DeviceName;
}

inline const BITMAPINFOHEADER& VideoCapture::GetBitmapInfoHeader() const
{
        return m_BmiHeader;
}

#endif VideoCapture_h

