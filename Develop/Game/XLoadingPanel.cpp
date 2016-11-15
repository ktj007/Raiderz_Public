#include "stdafx.h"
#include "XLoadingPanel.h"

XLoadingPanel::XLoadingPanel( const char* szName/*=NULL*/, MWidget* pParent/*=NULL*/, MListener* pListener/*=NULL*/ ) : MPanel(szName, pParent, pListener)
{
	if (pParent)
		SetRect(0, 0, pParent->GetRect().w, pParent->GetRect().h);

	m_pBackGroundPanel = new MPanel(NULL, this, this);
	m_pBackGroundPanel->SetRect(0, 0, GetRect().w, GetRect().h);
//	m_pBackGroundPanel->SetBorderStyle(MBS_NONE);
	m_pBackGroundPanel->ShowBorder( false);
//	m_pBackGroundPanel->SetBackgroundColor(MCOLOR(0xFF000000));
	m_pBackGroundPanel->SetPosition(0,0);

	m_pBackGround=new MPicture(NULL,this,this);	// 직접그린다.

	m_pBitmap = new MBitmapR3();
	wchar_t filename[_MAX_PATH];
	swprintf_s(filename, PATH_UI L"loading/loading%02d.jpg", RandomNumber(0, XCONST::LOADING_IMAGE_COUNT-1));
	m_pBitmap->Create( global.device, "loading", MLocale::ConvUTF16ToAnsi(filename).c_str());

	int bw = (GetRect().w * m_pBitmap->GetHeight()) / GetRect().h;
	if (bw != m_pBitmap->GetWidth())
	{
		float tex_w = (float)bw / m_pBitmap->GetWidth();
		if (tex_w < 1.0f)
		{

			m_pBitmap->SetTexCoord(0.0f + (1.0f - tex_w) / 2.0f, 0.0f, 1.0f - (1.0f - tex_w) / 2.0f, 1.0f);
		}
	}

	int w = GetRect().w;
	int h = GetRect().h;
	int x = 0;
	int y = 0;

	m_pBackGround->SetBitmap(m_pBitmap);
	m_pBackGround->SetPosition(0, 0);
	m_pBackGround->SetRect(x, y, w, h);
	m_pBackGround->SetStretch(true);

// 	m_pTipTextArea = new MTextArea(3, "sh_tips_textarea", this, this);
// 	m_pTipTextArea->SetBounds(50, int(GetRect().h * 0.88f), GetRect().w - 100, 50);
// 	m_pTipTextArea->ChangeLookToNull();
// 	m_pTipTextArea->SetZOrder(MZ_TOP);

	if (global.ui)
	{
//		m_pTipTextArea->SetFont(global.ui->GetDefFont());
	}
}

XLoadingPanel::~XLoadingPanel( void )
{
//	SAFE_DELETE(m_pTipTextArea);
	SAFE_DELETE(m_pBackGround);
	SAFE_DELETE(m_pBitmap);
	SAFE_DELETE(m_pBackGroundPanel);
}

void XLoadingPanel::SetTip( const wchar_t* szTip )
{
	return;
//	m_pTipTextArea->SetText(szTip);
}