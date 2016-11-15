#include "StdAfx.h"
#include "XBandiVideoPlayer.h"
#include "BandiVideoLibrary.h"
#include "XBandiVideoTexture.h"

XBandiVideoPlayer::XBandiVideoPlayer(void)
: m_hWnd(0)
, m_pDevice(NULL)
, m_pBandiVideoTexture(NULL)
{
	m_nWidth = 0;
	m_nHeight = 0;
}

XBandiVideoPlayer::~XBandiVideoPlayer(void)
{
	if(m_pBandiVideoTexture) delete m_pBandiVideoTexture; m_pBandiVideoTexture = NULL;
}

bool XBandiVideoPlayer::Init( HWND nHwnd, rs3::RDeviceD3D* pDevice )
{
	if (m_BandiVideoLibrary.IsCreated() == TRUE) return true;

	m_pDevice = pDevice;

	if (FAILED(m_BandiVideoLibrary.Create(BANDIVIDEO_DLL_FILE_NAME)))
	{
		_ASSERT(0);
		return false;
	}

	if (FAILED(m_BandiVideoLibrary.Verify("BANDISOFT-TRIAL-200809", "e1b03d86")))
	{
		//_ASSERT(0);
		return false;
	}

	return true;
}

bool XBandiVideoPlayer::Open(const wchar_t* path_name, bool async)
{
	if (m_BandiVideoLibrary.IsCreated() == FALSE) return false;

	if (FAILED(m_BandiVideoLibrary.Open(path_name, (async ? TRUE : FALSE))))
	{
		ASSERT(0);
		return false;
	}

	if(FAILED(m_BandiVideoLibrary.GetVideoInfo(m_video_info)))
	{
		ASSERT(0);
		return false;
	}

	if (XGETCFG_AUDIO_MUTE || XGETCFG_AUDIO_MUTEGENERICSOUND)
	{
		m_BandiVideoLibrary.SetVolume(0);
	}

	return SUCCEEDED(m_BandiVideoLibrary.Play());
}

void XBandiVideoPlayer::Play()
{
	m_BandiVideoLibrary.Seek(0, BVL_SEEK_TIME);
	m_BandiVideoLibrary.Play();
}

void XBandiVideoPlayer::Pause()
{
	BVL_STATUS	status;
	if(SUCCEEDED(m_BandiVideoLibrary.GetStatus(status)))
	{
		m_BandiVideoLibrary.Pause(status != BVL_STATUS_PAUSED);
	}
}

void XBandiVideoPlayer::Stop()
{
	m_BandiVideoLibrary.Stop();
}

void XBandiVideoPlayer::Close()
{
	m_BandiVideoLibrary.Close();
}

static int vid_y = 0;
void XBandiVideoPlayer::RenderScene()
{
	// Draw small screen.
	if(m_pBandiVideoTexture) m_pBandiVideoTexture->Draw(100, 100, 125, 100);
}

rs3::RTexture* XBandiVideoPlayer::GetVideoTexture()
{
	if (m_BandiVideoLibrary.IsCreated() == FALSE) return NULL;

	BVL_STATUS	status;
	m_BandiVideoLibrary.GetStatus(status);
	if (status == BVL_STATUS_PLAYEND)
	{
		m_BandiVideoLibrary.Seek(0, BVL_SEEK_TIME);
		m_BandiVideoLibrary.Play();
	}

	if (m_BandiVideoLibrary.IsNextFrame())
	{
		if (m_pBandiVideoTexture == NULL)
		{
			//BV_DEVICE_DX9 bvd = { m_pd3d9, m_pd3dDevice, m_hWnd };
			m_pBandiVideoTexture = new XBandiVideoTexture(m_pDevice);
			if(!m_pBandiVideoTexture || FAILED(m_pBandiVideoTexture->Open(m_video_info.width , m_video_info.height)))
			{
				if(m_pBandiVideoTexture) delete m_pBandiVideoTexture; m_pBandiVideoTexture = NULL;
				return NULL;
			}
		}

		INT		pitch;
		BYTE*	buf = m_pBandiVideoTexture->Lock(pitch);
		if(buf)
		{
			// Get frame
			BVL_FRAME frame;
			frame.frame_buf = buf;
			frame.frame_buf_size = m_video_info.height*pitch;
			frame.pitch = pitch;
			frame.width = m_video_info.width;
			frame.height = m_video_info.height;
			frame.pixel_format = m_pBandiVideoTexture->GetFormat();

			m_BandiVideoLibrary.GetFrame(frame, TRUE);

			m_pBandiVideoTexture->Unlock();

			m_current_time = frame.frame_time;
			m_current_frame = frame.frame_number;
		}
	}

	return m_pBandiVideoTexture ? (rs3::RTexture*)m_pBandiVideoTexture->GetObject() : NULL;
}

HRESULT XBandiVideoPlayer::OnResetDevice( int nWidth, int nHeight)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;

	return S_OK;
}