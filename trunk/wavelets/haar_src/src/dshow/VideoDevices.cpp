

#include "stdafx.h"
#include "videodevices.h"


HRESULT VideoDevices::Enumerate(std::vector<std::wstring>& names)
{        
        names.clear();
        
        ICreateDevEnum *pSysDevEnum = NULL;
        HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
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
                                if (SUCCEEDED(hr)) 
                                        names.push_back(std::wstring(varName.bstrVal));                                 
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
        return hr;
}
