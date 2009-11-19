// VidCapDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VidCap.h"
#include "VidCapDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVidCapDlg dialog
CVidCapDlg::CVidCapDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVidCapDlg::IDD, pParent)
        , m_bIsRunning(FALSE)
        , m_strVideoParams(_T("video:"))
        , m_strStatus(_T("status:"))
        , m_strFpsRate(_T("10fps"))
        , m_nTimer(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVidCapDlg::DoDataExchange(CDataExchange* pDX)
{
        CDialog::DoDataExchange(pDX);
        DDX_Control(pDX, IDC_ADAPTERS_COMBO, m_VideoAdaptorsCombo);
        DDX_Control(pDX, IDC_FPS_SLIDER, m_FpsSlider);
        DDX_Text(pDX, IDC_VIDEO_PARAMS_STATIC, m_strVideoParams);
        DDX_Text(pDX, IDC_STATUS_STATIC, m_strStatus);
        DDX_Text(pDX, IDC_FPS_STATIC, m_strFpsRate);
        DDX_Control(pDX, IDC_IMAGE_STATIC, m_ImageStatic);
        DDX_Control(pDX, IDC_START_BUTTON, m_StartButton);
}

BEGIN_MESSAGE_MAP(CVidCapDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
        ON_BN_CLICKED(IDC_ENUM_ADAPTORS_BUTTON, &CVidCapDlg::OnBnClickedEnumAdaptorsButton)
        ON_BN_CLICKED(IDC_INIT_AI_BUTTON, &CVidCapDlg::OnBnClickedInitAiButton)
        ON_BN_CLICKED(IDC_START_BUTTON, &CVidCapDlg::OnBnClickedStartButton)
        ON_WM_VSCROLL()
        ON_WM_TIMER()
END_MESSAGE_MAP()


// CVidCapDlg message handlers

BOOL CVidCapDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
        m_FpsSlider.SetRange(1, 25);
        m_FpsSlider.SetPos(10);
        m_nTimerInterval = 1000 / 10;                    

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CVidCapDlg::OnPaint()
{
	if (IsIconic())
	{
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
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CVidCapDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CVidCapDlg::OnBnClickedEnumAdaptorsButton()
{
        m_VideoAdaptorsCombo.ResetContent();
        std::vector<std::wstring> adaptors;
        HRESULT hr = VideoDevices::Enumerate(adaptors);
        TRACE(L"VideoDevices::Enumerate() = 0x%X\n", hr);
        if (adaptors.size() > 0) {
                for (unsigned int i = 0; i < (unsigned int)adaptors.size(); i++)
                        m_VideoAdaptorsCombo.AddString(adaptors[i].c_str());
                m_VideoAdaptorsCombo.SetCurSel(0);
        }        
}

void CVidCapDlg::OnBnClickedInitAiButton()
{
        m_HaarDetector.clear_ai_classifiers();
        m_HaarDetector.unload_skin_filter();
        m_HaarDetector.clear_object_sizes();        

        int res = 0;
        if (IsFileExists(L"skin.nn")) {
                res = m_HaarDetector.load_skin_filter(L"skin.nn");
                if (res != 0) {
                        CString err;
                        err.Format(L"Failed to load skin.nn: %d", res);
                        MessageBox(err, L"Error");
                }
        }

        m_HaarDetector.add_object_size(19, 19);
        m_HaarDetector.add_object_size(23, 23);
        m_HaarDetector.add_object_size(27, 27);

        UINT nClassifiers = 0;
        for (unsigned int i = 1; i < MAXINT; i++) {
                wchar_t strClassifier[256];
                wchar_t strFeatures[256];
                swprintf(strClassifier, 256, L"classifier%d.nn", i);
                swprintf(strFeatures, 256, L"features%d.txt", i);
                if (IsFileExists(strClassifier) && 
                    IsFileExists(strFeatures)) {
                            res = m_HaarDetector.add_ai_classifier(strClassifier, strFeatures);
                            if (res == 0) {
                                    nClassifiers++;
                            }
                            else {
                                    CString err;
                                    err.Format(L"Failed to load %d classifier: %d", i, res);
                                    MessageBox(err, L"Error");
                                    break;
                            }

                }
                else
                        break;
        }

        CString str;
        str.Format(L"Loaded %d classifiers.", nClassifiers);
        MessageBox(str, L"Success!");
}

void CVidCapDlg::OnBnClickedStartButton()
{        
        if (m_bIsRunning == FALSE && m_VideoAdaptorsCombo.GetCount() > 0) {
                CString str;
                m_VideoAdaptorsCombo.GetLBText(m_VideoAdaptorsCombo.GetCurSel(), str);

                m_VC = new VideoCapture();
                int res = m_VC->Connect(str);
                if (res != 0) {
                        CString err;
                        err.Format(L"Failed in connection to: %s", str);
                        MessageBox(err, L"Error");
                        delete m_VC;
                        return;
                }

                m_strVideoParams.Format(L"%dx%d %dbpp", m_VC->GetBitmapInfoHeader().biWidth,
                                                        m_VC->GetBitmapInfoHeader().biHeight,
                                                        m_VC->GetBitmapInfoHeader().biBitCount);
                UpdateData(FALSE);

                //if (m_VC->GetBitmapInfoHeader().biWidth != 640 ||
                //    m_VC->GetBitmapInfoHeader().biHeight != 480 ||
                //    m_VC->GetBitmapInfoHeader().biBitCount != 24) {                        
                //        CString err;
                //        err.Format(L"GUI suports only 640x480 24bpp video stream", str);
                //        MessageBox(err, L"Unsupported stream");
                //        delete m_VC;
                //        return;
                //}

                res = m_VC->Start();
                if (res != 0) {
                        CString err;
                        err.Format(L"Failed starting device: %s", str);
                        MessageBox(err, L"Error");
                        delete m_VC;
                        return;
                }

                m_nTimer = SetTimer(1, m_nTimerInterval, 0);

                pVideoFrame = ::new Gdiplus::Bitmap(m_VC->GetBitmapInfoHeader().biWidth,
                                                  m_VC->GetBitmapInfoHeader().biHeight,
                                                  PixelFormat24bppRGB);

                m_ImgResize.init(320, 240, 0.125);
                m_MotionDetector.init(m_ImgResize.gety()->width(), m_ImgResize.gety()->height());
                m_HaarDetector.close();
                res = m_HaarDetector.init(m_ImgResize.gety()->width(), m_ImgResize.gety()->height());
                if (res != 0) {
                        CString err;
                        err.Format(L"Failed initializing Haar Detector: %d", res);
                        MessageBox(err, L"Error");
                }

                m_dTicks = 0.0;
                m_nFramesProcessed = 0;
                
                m_bIsRunning = TRUE;
                m_StartButton.SetWindowTextW(L"Stop");
                TRACE(L"started capture...\n");
        }
        else if (m_bIsRunning == TRUE){
                KillTimer(m_nTimer);
                m_nTimer = 0;
                m_VC->Stop();
                delete m_VC;
                ::delete pVideoFrame;
                m_bIsRunning = FALSE;
                m_StartButton.SetWindowTextW(L"Start");
                TRACE(L"...stoped capture\n");
        }
}

void CVidCapDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
        // TODO: Add your message handler code here and/or call default
        BOOL res = pScrollBar->IsKindOf(RUNTIME_CLASS(CSliderCtrl));
        if (res && (nSBCode == SB_THUMBPOSITION ||
                    nSBCode == SB_LEFT ||
                    nSBCode == SB_LINELEFT ||
                    nSBCode == SB_LINERIGHT ||
                    nSBCode == SB_PAGELEFT ||
                    nSBCode == SB_PAGERIGHT ||
                    nSBCode == SB_RIGHT)) {
                m_nTimerInterval = (UINT)(1000.0 / (double)m_FpsSlider.GetPos());
                m_strFpsRate.Format(L"%dfps", m_FpsSlider.GetPos());

                if (m_bIsRunning == TRUE) {                                
                        KillTimer(m_nTimer);                
                        m_nTimer = SetTimer(1, m_nTimerInterval, 0);                        
                        m_dTicks = 0.0;                        
                        m_nFramesProcessed = 0;                        
                }

                UpdateData(FALSE);
        }

        CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}



void CVidCapDlg::OnTimer(UINT_PTR nIDEvent)
{
        // TODO: Add your message handler code here and/or call default
        if (m_bIsRunning = TRUE) {
                const BYTE* pData = m_VC->GrabFrame();
                if (pData == 0)
                        return;
                else {
                        SetBitmapData(pVideoFrame, pData);
                        RECT r;
                        m_ImageStatic.GetClientRect(&r);
                        Gdiplus::Graphics g(m_ImageStatic.GetDC()->m_hDC);

                        INT nObjects = 0;

                        Tic();

                        m_ImgResize.resize(pData);
                        m_MotionDetector.detect(*m_ImgResize.gety(), m_HaarDetector);
                        //m_MotionDetector.get_motion_vector().print(L"motion.txt");
                        if (m_HaarDetector.status() == 0) {                                
                                nObjects = m_HaarDetector.detect_objects(m_ImgResize.gety(),
                                                                         m_ImgResize.getr(),
                                                                         m_ImgResize.getg(),
                                                                         m_ImgResize.getb(),
                                                                         &m_MotionDetector.get_motion_vector());
                        }                        

                        m_dTicks += Toc();
                        m_nFramesProcessed++;

                        if(m_nFramesProcessed >= m_FpsSlider.GetPos()) {
                                m_strStatus.Format(L"detection speed: %.2ffps", 1000.0 / (m_dTicks / double(m_nFramesProcessed)));
                                m_dTicks = 0.0;
                                m_nFramesProcessed = 0;
                        }

                        Gdiplus::Graphics mem_g(pVideoFrame);
                        Gdiplus::Pen redPen(Gdiplus::Color(255, 255, 0, 0), 2.5f);
                        for (int i = 0; i < nObjects; i++) {                                
                                const Rect* r = m_HaarDetector.get_detected_object_rect(i);
                                RECT rect;
                                float zoom = 0.125f;
                                rect.left = int((float)r->left * (1.0f / zoom));
                                rect.top = int((float)r->top * (1.0f / zoom));
                                rect.right = int((float)(r->right + 1) * (1.0f / zoom) - (float)r->left * (1.0f / zoom));
                                rect.bottom = int((float)(r->bottom + 1) * (1.0f / zoom) - (float)r->top * (1.0f / zoom));
                                mem_g.DrawRectangle(&redPen, rect.left, rect.top, rect.right, rect.bottom);                                
                        }

                        g.DrawImage(pVideoFrame, Gdiplus::Rect(0, 0, r.right, r.bottom));

                        UpdateData(FALSE);
                }
        }

        CDialog::OnTimer(nIDEvent);
}



int CVidCapDlg::SetBitmapData(Gdiplus::Bitmap* pBitmap, const BYTE* pData)
{
        Gdiplus::BitmapData bitmapData;
        bitmapData.Width = pBitmap->GetWidth();
        bitmapData.Height = pBitmap->GetHeight();
        bitmapData.Stride = 3 * bitmapData.Width;
        bitmapData.PixelFormat = pBitmap->GetPixelFormat();
        bitmapData.Scan0 = (VOID *)pData;
        bitmapData.Reserved = NULL;
        Gdiplus::Status s = pBitmap->LockBits(&Gdiplus::Rect(0, 0, pBitmap->GetWidth(), pBitmap->GetHeight()),
                                              Gdiplus::ImageLockModeWrite | Gdiplus::ImageLockModeUserInputBuf,
                                              PixelFormat24bppRGB, &bitmapData);
        if (s == Gdiplus::Ok)
                pBitmap->UnlockBits(&bitmapData);
        return s;
}

void CVidCapDlg::Tic()
{
        QueryPerformanceFrequency(&m_nFreq);
        QueryPerformanceCounter(&m_nBeginTime);
}

double CVidCapDlg::Toc()
{
        LARGE_INTEGER nEndTime;
        double nCalcTime;

        QueryPerformanceCounter(&nEndTime);
        nCalcTime = double((nEndTime.QuadPart - m_nBeginTime.QuadPart) * 1000) / double(m_nFreq.QuadPart);

        return nCalcTime;
}

BOOL CVidCapDlg::IsFileExists(LPTSTR lpszFileName)
{
        FILE* fp = _wfopen(lpszFileName, L"rt");
        if (fp != 0) {
                fclose(fp);
                return TRUE;
        }
        else 
                return FALSE;        
}
