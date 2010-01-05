
#ifndef CAPTURE_h
#define CAPTURE_h

// Application-defined message to notify app of filtergraph events
#define WM_GRAPHNOTIFY  WM_APP+1

void vcGetCaptureDevices(CComboBox& adaptersBox);
HRESULT vcCaptureVideo(HWND msgWindow, HWND prvWindow, unsigned int devIndex = 1); //devIndex = 1,2,3 ...
void vcStopCaptureVideo();

HRESULT vcHandleGraphEvent(void);
HRESULT vcChangePreviewState(int nShow);


#endif