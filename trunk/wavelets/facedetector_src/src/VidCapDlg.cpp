// VidCapDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VidCap.h"
#include "VidCapDlg.h"

#include "capture.h"
#include "samplegrab.h"

#include "cvlib\vec2d.h"
#include "cvlib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CVidCapDlg dialog



CVidCapDlg::CVidCapDlg(CWnd* pParent /*=NULL*/)
                : CDialog(CVidCapDlg::IDD, pParent)
                , m_ResizeRatio(0.125)
                , m_Width(640), m_Height(480), m_Channels(3)
                , m_FaceRectRatio(8.5)
                , m_nTimer(0), m_TimerInterval(1000)
                , m_fDetectBox(FALSE)
                , m_cvInfoStatic(_T("cvInfo"))
                , m_fDetectionTime(_T(""))
                , m_TakeSnapshot(false)
                , pBmpEncoder(GUID_NULL)
{
        m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVidCapDlg::DoDataExchange(CDataExchange* pDX)
{
        CDialog::DoDataExchange(pDX);
        DDX_Control(pDX, IDC_PRV_STATIC, m_PrvStatic);
        DDX_Control(pDX, IDC_ADAPTORS_COMBO, m_AdapterCombo);
        DDX_Text(pDX, IDC_SAMPLEINTERVAL_EDIT, m_TimerInterval);
        DDV_MinMaxUInt(pDX, m_TimerInterval, 10, 10000);
        DDX_Control(pDX, IDC_RUN_BUTTON, m_RunButton);
        DDX_Control(pDX, IDC_CAPIMG_STATIC, m_CapImgStatic);
        DDX_Control(pDX, IDC_VIDINFO_STATIC, m_VideoFormat);
        DDX_Check(pDX, IDC_FDETECT_CHECK, m_fDetectBox);
        DDX_Text(pDX, IDC_CVINFO_STATIC, m_cvInfoStatic);
        DDX_Text(pDX, IDC_DETECTIONMS_STATIC, m_fDetectionTime);
}

BEGIN_MESSAGE_MAP(CVidCapDlg, CDialog)
        ON_MESSAGE(WM_GRAPHNOTIFY, OnGraphMessage)
        ON_WM_PAINT()
        ON_WM_QUERYDRAGICON()
        //}}AFX_MSG_MAP
        ON_BN_CLICKED(IDC_ENUMADAPTORS_BUTTON, &CVidCapDlg::OnBnClickedEnumadaptorsButton)
        ON_BN_CLICKED(IDC_RUN_BUTTON, &CVidCapDlg::OnBnClickedRunButton)
        ON_WM_TIMER()
        ON_WM_CLOSE()
        ON_WM_WINDOWPOSCHANGED()
        ON_BN_CLICKED(IDC_FDETECT_CHECK, &CVidCapDlg::OnBnClickedFdetectCheck)
        ON_STN_DBLCLK(IDC_CAPIMG_STATIC, &CVidCapDlg::OnStnDblclickCapimgStatic)
        ON_STN_DBLCLK(IDC_PRV_STATIC, &CVidCapDlg::OnStnDblclickPrvStatic)
END_MESSAGE_MAP()


// CVidCapDlg message handlers
LRESULT CVidCapDlg::OnGraphMessage(WPARAM wParam, LPARAM lParam)
{
        HRESULT hr = vcHandleGraphEvent();
        TRACE(L" WM_GRAPH 0x%X\n", hr);
        return 0;
}

BOOL CVidCapDlg::OnInitDialog()
{
        CDialog::OnInitDialog();

        // Set the icon for this dialog.  The framework does this automatically
        //  when the application's main window is not a dialog
        SetIcon(m_hIcon, TRUE);			// Set big icon
        SetIcon(m_hIcon, FALSE);		// Set small icon

        // TODO: Add extra initialization here

        // Initialize COM
        if (FAILED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED))) {
                MessageBox(L"CoInitialize Failed!", L"COM error");
                m_RunButton.EnableWindow(FALSE);
                return TRUE;
        }

        m_CapturedFace = ::new Gdiplus::Bitmap(19, 19, PixelFormat24bppRGB);

        if (GetEncoderClsid(L"image/jpeg", &pBmpEncoder) < 0) {
                MessageBox(L"Failed to get image/bmp encoder", L"warning");
        }


        return TRUE;  // return TRUE  unless you set the focus to a control
}

void CVidCapDlg::OnClose()
{
        // TODO: Add your message handler code here and/or call default
        KillTimer(m_nTimer);
        vcStopCaptureVideo();
        CoUninitialize();
        
        ::delete m_CapturedFace;

        CDialog::OnClose();
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CVidCapDlg::OnPaint()
{
        if (IsIconic()) {
                CPaintDC dc(this); // device context for painting

                SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

                // Center icon in client rectangle
                int cxIcon = GetSystemMetrics(SM_CXICON);
                int cyIcon = GetSystemMetrics(SM_CYICON);
                CRect rect;
                GetClientRect(&rect);
                int x = (rect.Width() - cxIcon + 1) / 2;
                int y = (rect.Height() - cyIcon + 1) / 2;

                // Draw the icon
                dc.DrawIcon(x, y, m_hIcon);
        } else {
                CDialog::OnPaint();
        }
}


int CVidCapDlg::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
        UINT  num = 0;          // number of image encoders
        UINT  size = 0;         // size of the image encoder array in bytes

        Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

        Gdiplus::GetImageEncodersSize(&num, &size);
        if (size == 0)
                return -1;  // Failure

        pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
        if (pImageCodecInfo == NULL)
                return -1;  // Failure

        Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

        for (UINT j = 0; j < num; ++j) {
                if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
                        *pClsid = pImageCodecInfo[j].Clsid;
                        free(pImageCodecInfo);
                        return j;  // Success
                }
        }

        free(pImageCodecInfo);
        return -1;  // Failure
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CVidCapDlg::OnQueryDragIcon()
{
        return static_cast<HCURSOR>(m_hIcon);
}


void CVidCapDlg::OnBnClickedEnumadaptorsButton()
{
        //Enum adaptors
        vcGetCaptureDevices(m_AdapterCombo);
}

void CVidCapDlg::OnBnClickedRunButton()
{
        UpdateData();
        //unsigned char* data = new unsigned char[640*480*3];
        //cvDetect(data);
        //delete[] data;
        //return;

        HRESULT hr;
        if (m_nTimer == 0) {
                //Run capture
                hr = vcCaptureVideo(m_hWnd, m_PrvStatic.m_hWnd, m_AdapterCombo.GetCurSel() + 1);
                if (hr != S_OK) {
                        vcStopCaptureVideo();
                        return;
                }

                CString str;
                str.Format(L"Video output: %dx%d %dbpp", sgGetDataWidth(), sgGetDataHeight(), 8 * sgGetDataChannels()); 
                m_VideoFormat.SetWindowTextW(str);

                //Setup Timer
                if (sgGetDataWidth() == m_Width && sgGetDataHeight() == m_Height && sgGetDataChannels() == m_Channels) {
                        m_nTimer = SetTimer(1, m_TimerInterval, 0);
                        m_FpsRate = 0.0;
                        m_Ms = 0;
                        m_MsPerFrame = 0;
                        m_FramesProcessed = 0;
                        m_TotalFrames = 1000 / m_TimerInterval;
                        if (m_TotalFrames == 0)
                                m_TotalFrames = 1;
                }

                m_RunButton.SetWindowTextW(L"Stop");
        } else {
                //Close Timer
                KillTimer(m_nTimer);
                m_nTimer = 0;
                m_RunButton.SetWindowTextW(L"Run");

                m_VideoFormat.SetWindowTextW(L"Video output");
                //Close Capture
                vcStopCaptureVideo();
        }
}

void CVidCapDlg::OnTimer(UINT_PTR nIDEvent)
{
        // TODO: Add your message handler code here and/or call default
        SYSTEMTIME SystemTime;
        GetLocalTime(&SystemTime);
        TRACE(L" %d:%d:%d\n", SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond);

        unsigned char* pData = sgGrabData();
        if (pData != 0)
                DrawData(pData);

        CDialog::OnTimer(nIDEvent);
}


void CVidCapDlg::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
        CDialog::OnWindowPosChanged(lpwndpos);

        // TODO: Add your message handler code here
        vcChangePreviewState(!IsIconic());
}

void CVidCapDlg::DrawData(unsigned char *pData)
{
        Gdiplus::Bitmap* pBitmap = sgGetBitmap();
        if (pBitmap == 0)
                return;        

        if (m_TakeSnapshot == true) {
                m_TakeSnapshot = false;
                sndPlaySound(L"snap.wav", SND_ASYNC);
                if (pBmpEncoder != GUID_NULL) {
                        wchar_t FileName[_MAX_PATH] = L"";
                        for (unsigned int i = 1; i < 0xFFFFFFFF; i++) {
                                swprintf_s(FileName, _MAX_PATH, L"Snapshot %04d.jpg", i);
                                FILE* fp = _wfopen(FileName, L"rb");
                                if (fp == 0) {
                                        pBitmap->Save(FileName, &pBmpEncoder);
                                        break;
                                }
                                else
                                        fclose(fp);

                        }                        
                }
        }

        RECT r;
        m_CapImgStatic.GetClientRect(&r);
        Gdiplus::Graphics g(m_CapImgStatic.GetDC()->m_hDC);

        //detect faces
        tic();
        int nFaces = cvDetect(pData);        
        m_Ms += (unsigned int)toc();
        m_FramesProcessed++;

        TRACE(L" nFaces: %d\n", nFaces);        
        if (nFaces < 0)                 
                nFaces = 0;        

        Gdiplus::Graphics mem_g(pBitmap);
        Gdiplus::Pen redPen(Gdiplus::Color(255, 255, 0, 0), 2.5f);
        for (int i = 0; i < nFaces; i++) {
                RECT rect;
                cvFaceRect(i, rect);
                mem_g.DrawRectangle(&redPen, rect.left, rect.top, rect.right, rect.bottom);
                TRACE(L" face coords: %d %d  %d %d", rect.left, rect.top, rect.right, rect.bottom);
        }

        if (nFaces > 0) {                
                SetCapturedImageData(m_CapturedFace, *cvGetFace(0));
                mem_g.DrawImage(m_CapturedFace, 
                                Gdiplus::Rect(10, 10, (int)((double)m_Width / m_FaceRectRatio), (int)((double)m_Width / m_FaceRectRatio)),
                                0, 0, m_CapturedFace->GetWidth(), m_CapturedFace->GetHeight(), 
                                Gdiplus::UnitPixel);
        }

        g.DrawImage(pBitmap, Gdiplus::Rect(0, 0, r.right, r.bottom));

        if (m_FramesProcessed >= m_TotalFrames) {                
                m_MsPerFrame = m_Ms / m_TotalFrames;
                m_FpsRate = 1000.0 / double(m_MsPerFrame);
                m_FramesProcessed = 0;
                m_Ms = 0;
        }

        m_fDetectionTime.Format(L"detection time: %dms (%.2lffps)  motion: %.2f, skin: %.2f", m_MsPerFrame, m_FpsRate, 
                                                                                              cvMotionAmount(),
                                                                                              cvSkinAmount());        
        UpdateData(FALSE);
}

int CVidCapDlg::SetCapturedImageData(Gdiplus::Bitmap* pBitmap, const vec2D& pData)
{        
        Gdiplus::BitmapData bitmapData;
        Gdiplus::Status s = pBitmap->LockBits(&Gdiplus::Rect(0, 0, pBitmap->GetWidth(), pBitmap->GetHeight()), Gdiplus::ImageLockModeWrite, 
                                              PixelFormat24bppRGB, &bitmapData);
        if (s == Gdiplus::Ok) {
                unsigned int Width = (pData.width() < pBitmap->GetWidth()) ? pData.width() : pBitmap->GetWidth();
                unsigned int Height = (pData.height() < pBitmap->GetHeight()) ? pData.width() : pBitmap->GetHeight();

                unsigned char* Pixels = (unsigned char *)bitmapData.Scan0;
                for(unsigned int y = 0; y < Height; y++) {
                        for(unsigned int x = 0; x < Width; x++) {
                                unsigned char c = (unsigned char)(255.0f * pData(y, x));
                                Pixels[3*x] = c;
                                Pixels[3*x+1] = c;
                                Pixels[3*x+2] = c;                                
                        }
                        Pixels += bitmapData.Stride;
                }

                pBitmap->UnlockBits(&bitmapData);
        }

        return s;
}

void CVidCapDlg::OnBnClickedFdetectCheck()
{
        // TODO: Add your control notification handler code here
        UpdateData();
        if (m_fDetectBox == TRUE) {
                double scales[3] = {0.86, 0.73, 0.6};
                cvSetScales(scales, 3);
                cvInit(m_Width, m_Height, 19, 19, m_ResizeRatio);
                int res = cvInitAI(L"face.nn", L"pca.nn", L"skin.nn", L"preflt.nn");
                TRACE(L" cvInitNN = %d\n", res);
                if (res < 0) {
                        MessageBox(L"Failed to load classifiers.", L"error");
                        m_fDetectBox = false;
                        UpdateData(false);
                }

                m_cvInfoStatic = CString(cvInfo());
        } else
                m_cvInfoStatic = CString(L"cvInfo");
        UpdateData(false);

}

void CVidCapDlg::OnStnDblclickCapimgStatic()
{
        m_TakeSnapshot = true;
}

void CVidCapDlg::OnStnDblclickPrvStatic()
{
        CFileDialog dlg(true);
        dlg.GetOFN().lpstrFilter = L"Image Files\0*.bmp;*.jpg;*.jpeg\0\0";
        if (dlg.DoModal() == IDOK) {
                Gdiplus::Bitmap* pBitmap = ::new Gdiplus::Bitmap(dlg.GetPathName());
                if (pBitmap->GetLastStatus() == Gdiplus::Ok) {
                        if (pBitmap->GetWidth() == m_Width &&
                                        pBitmap->GetHeight() == m_Height &&
                                        pBitmap->GetPixelFormat() == PixelFormat24bppRGB) {

                                RECT r;
                                m_CapImgStatic.GetClientRect(&r);
                                Gdiplus::Graphics g(m_CapImgStatic.GetDC()->m_hDC);

                                int nFaces = 0;

                                Gdiplus::BitmapData bitmapData;
                                Gdiplus::Status s = pBitmap->LockBits(&Gdiplus::Rect(0, 0, m_Width, m_Height), Gdiplus::ImageLockModeRead, 
                                                                      PixelFormat24bppRGB, &bitmapData);
                                if (s == Gdiplus::Ok) {
                                        //detect faces
                                        tic();
                                        nFaces = cvDetect((unsigned char *)bitmapData.Scan0);
                                        __int64 ms = toc();
                                        TRACE(L" nFaces: %d\n", nFaces);
                                        if (nFaces < 0)
                                                nFaces = 0;

                                        pBitmap->UnlockBits(&bitmapData);

                                        m_fDetectionTime.Format(L"detection time: %dms (%.2ffps)", (int)ms, 1000.0f / float(ms));
                                        UpdateData(false);                                        
                                }

                                Gdiplus::Graphics mem_g(pBitmap);
                                Gdiplus::Pen redPen(Gdiplus::Color(255, 255, 0, 0), 2.5f);
                                for (int i = 0; i < nFaces; i++) {
                                        RECT rect;
                                        cvFaceRect(i, rect);
                                        mem_g.DrawRectangle(&redPen, rect.left, rect.top, rect.right, rect.bottom);
                                        TRACE(L" face coords: %d %d  %d %d", rect.left, rect.top, rect.right, rect.bottom);
                                }
                                if (nFaces > 0) {                
                                        SetCapturedImageData(m_CapturedFace, *cvGetFace(0));
                                        mem_g.DrawImage(m_CapturedFace, 
                                                        Gdiplus::Rect(10, 10, (int)((double)m_Width / m_FaceRectRatio), (int)((double)m_Width / m_FaceRectRatio)),
                                                        0, 0, m_CapturedFace->GetWidth(), m_CapturedFace->GetHeight(), 
                                                        Gdiplus::UnitPixel);
                                }

                                g.DrawImage(pBitmap, Gdiplus::Rect(0, 0, r.right, r.bottom));

                        } else
                                MessageBox(L"Loaded bitmap not in the right format", L"wrong format.");
                        ::delete pBitmap;
                } else
                        MessageBox(L"Failed to load bitmap", L"error.");

        }
}
