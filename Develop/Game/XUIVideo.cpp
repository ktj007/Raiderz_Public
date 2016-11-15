#include "stdafx.h"
#include "XUIVideo.h"
#include "XBandiVideoPlayer.h"

// XUIVideo
XUIVideo::XUIVideo( const char* szName, MWidget* pParent, MListener* pListener)
: MWidget( szName, pParent, pListener)
{
	DECLARE_DEFAULT_LOOK( XUIVideoLook);

	bandi_video = new XBandiVideoPlayer();

	m_bEnableFocus = true;
}


// ~XUIVideo
XUIVideo::~XUIVideo()
{
	SAFE_DELETE(bandi_video);
}


// OnEvent
bool XUIVideo::OnEvent( MEvent* pEvent, MListener* pListener)
{
	if ( MWidget::OnEvent( pEvent, pListener) == true)		return true;

	return false;
}


void XUIVideo::OnRun( DWORD dwCurrTime)
{
	MWidget::OnRun( dwCurrTime);
}

bool XUIVideo::Open(const char* szName)
{
	if (bandi_video)
	{
		HWND nHwnd = RApplication::GetInstance().GetWindowHandle();
		rs3::RDeviceD3D* pDevice = global.device;

		wstring name = MLocale::ConvAnsiToUTF16(szName);

		bandi_video->Init(nHwnd, pDevice);
		bandi_video->Open(name.c_str());

		return true;
	}

	return false;
}

RTexture* XUIVideo::GetVideoTexture()
{
	if (bandi_video)
	{
		return bandi_video->GetVideoTexture();
	}

	return NULL;
}














// XUIVideoLook
XUIVideoLook::XUIVideoLook()
{
	m_nTextAlign = MAM_HCENTER | MAM_VCENTER;
	m_pBitmap = new MBitmapR3();
}


XUIVideoLook::~XUIVideoLook()
{
	if ( m_pBitmap != NULL)
	{
		// 텍스쳐는 시스템에서 알아서 지우니 여기선 NULL 처리만.
		m_pBitmap->Create(NULL, rs3::R_NONE);

		delete m_pBitmap;
		m_pBitmap = NULL;
	}
}

void XUIVideoLook::OnDrawBorder( MWidget* pWidget, MDrawContext* pDC)
{
	XUIVideo* pUIVideo = (XUIVideo*)pWidget;
	MRECT r = pUIVideo->GetClientRect();

	RTexture* pTexture = pUIVideo->GetVideoTexture();
	if (pTexture == NULL) return;


	rs3::RDevice* pDevice = REngine::GetDevicePtr();
	if (pDevice == NULL) return;

	m_pBitmap->Create(pDevice, pTexture);
	pDC->SetBitmap( m_pBitmap);
	pDC->Draw( r);
}


void XUIVideoLook::OnDraw( MWidget* pWidget, MDrawContext* pDC)
{
	//XUIVideo* pUIVideo = (XUIVideo*)pWidget;
	//if (pUIVideo)
	//{
	//	XBandiVideoPlayer* bandi_video = pUIVideo->GetBandi();
	//	bandi_video->GetVideoTexture();
	//	bandi_video->RenderScene();
	//}
}
