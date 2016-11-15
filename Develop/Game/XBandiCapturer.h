#pragma once

#include "../../../sdk/bandi_capture_library/v1.4.1/include/bandicap.h"

namespace rs3
{
	class RDeviceD3D;
}

class XBandiCapturer
{
private:
	HWND					m_hHandle;
	rs3::RDeviceD3D*		m_pDevice;
	bool					m_bInited;
	CBandiCaptureLibrary	m_bandiCaptureLibrary;

	void OnDrawCapture(bool bInRenderLoop);
	BCAP_CONFIG GetConfig();
	void MakeFolderName(wchar_t* szoutFolder);
	void MakePathName(wchar_t* szoutFileName);
	bool Init(HWND nHwnd, rs3::RDeviceD3D* pDevice);
	void Final();
	bool CheckInit();
public:
	XBandiCapturer() : m_bInited(false), m_hHandle(0), m_pDevice(NULL) {}
	~XBandiCapturer();

	bool Start();
	void Stop();
	void ToggleStart();
	void DrawCapture(bool bInRenderLoop);
	bool IsCapturing();
};