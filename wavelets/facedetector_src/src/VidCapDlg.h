// VidCapDlg.h : header file
//

#pragma once
#include "afxwin.h"

#include "cvlib\vec2d.h"


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
        double m_ResizeRatio;
        unsigned int m_Width;
        unsigned int m_Height;
        unsigned int m_Channels;
        double m_FaceRectRatio;

        UINT_PTR m_nTimer;
        double m_FpsRate;
        unsigned int m_MsPerFrame;
        unsigned int m_TotalFrames;
        unsigned int m_FramesProcessed;
        unsigned int m_Ms;        

        CStatic m_PrvStatic;
        CStatic m_CapImgStatic;
        CComboBox m_AdapterCombo;
        UINT m_TimerInterval;
        CButton m_RunButton;
        CStatic m_VideoFormat;
        BOOL m_fDetectBox;
        CString m_cvInfoStatic;
        bool m_TakeSnapshot;
        
        Gdiplus::Bitmap* m_CapturedFace;

        CLSID pBmpEncoder;
        int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

        void DrawData(unsigned char* pData);        
        int SetCapturedImageData(Gdiplus::Bitmap* pBitmap, const vec2D& pData);


        afx_msg LRESULT OnGraphMessage(WPARAM wParam, LPARAM lParam);
        afx_msg void OnBnClickedEnumadaptorsButton();
        afx_msg void OnBnClickedRunButton();
        afx_msg void OnTimer(UINT_PTR nIDEvent);
        afx_msg void OnClose();
        afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
        afx_msg void OnBnClickedFdetectCheck();

private:
        CString m_fDetectionTime;
        afx_msg void OnStnDblclickCapimgStatic();
        afx_msg void OnStnDblclickPrvStatic();
};
