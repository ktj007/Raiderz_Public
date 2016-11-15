#pragma once

#include "BandiVideoLibrary.h"

namespace rs3
{
	class RDeviceD3D;
}

class CBandiVideoLibrary;
class XBandiVideoTexture;
class XBandiVideoPlayer
{
private:

	// test
	int					m_nWidth;
	int					m_nHeight;

	HWND				m_hWnd;
	rs3::RDeviceD3D*	m_pDevice;
	CBandiVideoLibrary	m_BandiVideoLibrary;
	XBandiVideoTexture*	m_pBandiVideoTexture;

	LONGLONG			m_current_time;
	LONGLONG			m_current_frame;

	BVL_VIDEO_INFO		m_video_info;

public:
	XBandiVideoPlayer(void);
	virtual ~XBandiVideoPlayer(void);

	bool Init(HWND nHwnd, rs3::RDeviceD3D* pDevice);

	bool Open(const wchar_t* path_name, bool async = false);
	void Play();
	void Pause();
	void Stop();
	void Close();

	rs3::RTexture* GetVideoTexture();
	void RenderScene();

	HRESULT OnResetDevice( int nWidth, int nHeight);
};
