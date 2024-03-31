#include <Windows.h>
#include <assert.h>
#include <stdio.h>
#include <iostream>
#include <wininet.h>
#include <thread>
#include <chrono>
#pragma comment(lib,"Wininet.lib")

static GUID const CLSID_RadioManagementAPI = { 0x581333f6, 0x28db, 0x41be, { 0xbc, 0x7a, 0xff, 0x20, 0x1f, 0x12, 0xf3, 0xf6 } };
static GUID const CID_IRadioManager = { 0xdb3afbfb, 0x08e6, 0x46c6, { 0xaa, 0x70, 0xbf, 0x9a, 0x34, 0xc3, 0x0a, 0xb7 } };

typedef IUnknown IUIRadioInstanceCollection; /* Didn't bother rev-engineering this one... */
typedef DWORD _RADIO_CHANGE_REASON;

typedef struct IRadioManagerVtbl IRadioManagerVtbl;
typedef struct IRadioManager {
    IRadioManagerVtbl* lpVtbl;
} IRadioManager;
struct IRadioManagerVtbl {
    /* IUnknown */
    HRESULT(STDMETHODCALLTYPE* QueryInterface)(IRadioManager* This, GUID const* riid, LPVOID* ppvObj);
    ULONG(STDMETHODCALLTYPE* AddRef)(IRadioManager* This);
    ULONG(STDMETHODCALLTYPE* Release)(IRadioManager* This);
    /* IRadioManager (aka. `CUIRadioManager') */
    HRESULT(STDMETHODCALLTYPE* IsRMSupported)(IRadioManager* This, DWORD* pdwState);
    HRESULT(STDMETHODCALLTYPE* GetUIRadioInstances)(IRadioManager* This, IUIRadioInstanceCollection** param_1);
    HRESULT(STDMETHODCALLTYPE* GetSystemRadioState)(IRadioManager* This, int* pbEnabled, int* param_2, _RADIO_CHANGE_REASON* param_3);
    HRESULT(STDMETHODCALLTYPE* SetSystemRadioState)(IRadioManager* This, int bEnabled);
    HRESULT(STDMETHODCALLTYPE* Refresh)(IRadioManager* This);
    HRESULT(STDMETHODCALLTYPE* OnHardwareSliderChange)(IRadioManager* This, int param_1, int param_2);
};

int main() {
    while (true)
    {
        if (InternetCheckConnection("https://google.com", FLAG_ICC_FORCE_CONNECTION, 0))
        {
            std::cout << std::chrono::system_clock::now() << " Internet is connected" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(60));
        }
        else
        {
            IRadioManager* irm = NULL;
            auto hr = CoInitialize(NULL);
            hr = CoCreateInstance(CLSID_RadioManagementAPI, NULL, 4, CID_IRadioManager, (void**)&irm);
            if (irm == NULL)
            {
                continue;
            }
            std::cout << std::chrono::system_clock::now() << " Internet is not connected, restarting..." << std::endl;
            hr = irm->lpVtbl->SetSystemRadioState(irm, 1);
            std::this_thread::sleep_for(std::chrono::seconds(60));
            hr = irm->lpVtbl->SetSystemRadioState(irm, 0);
            std::cout << std::chrono::system_clock::now() << " Restarted" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(60));
            irm->lpVtbl->Release(irm);
            CoUninitialize();
        }
    }
    return 0;
}