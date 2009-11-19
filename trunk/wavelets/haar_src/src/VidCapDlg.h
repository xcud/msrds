// VidCapDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


#include "dshow\videodevices.h"
#include "dshow\videocapture.h"

#include "ai\common.h"
#include "ai\imageresize.h"
#include "ai\motiondetector.h"
#include "ai\haardetector.h"


// CVidCapDlg dialog
class CVidCapDlg : public CDialog
{
// Construction
public:
	CVidCapDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_VIDCAP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
        ImageResize m_ImgResize;
        MotionDetector m_MotionDetector;
        HaarDetector m_HaarDetector;
        std::vector<ObjectSize> m_ObjectSizes;

        BOOL m_bIsRunning;
        VideoCapture* m_VC;
        Gdiplus::Bitmap* pVideoFrame;
        UINT m_nTimerInterval;
        UINT m_nTimer;
        CComboBox m_VideoAdaptorsCombo;
        CStatic m_ImageStatic;
        CSliderCtrl m_FpsSlider;
        CString m_strVideoParams;
        CString m_strStatus;
        CString m_strFpsRate;
        CButton m_StartButton;

        int SetBitmapData(Gdiplus::Bitmap* pBitmap, const BYTE* pData);

        LARGE_INTEGER m_nFreq;
        LARGE_INTEGER m_nBeginTime;
        void Tic();
        double Toc();

        double m_dTicks;
        UINT m_nFramesProcessed;

        BOOL IsFileExists(LPTSTR lpszFileName);

public:
        afx_msg void OnBnClickedEnumAdaptorsButton();
        afx_msg void OnBnClickedInitAiButton();
        afx_msg void OnBnClickedStartButton();
        afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
        afx_msg void OnTimer(UINT_PTR nIDEvent);


};
