#include "StdAfx.h"
#include "XOnScreenDialog.h"
#include "MMainFrame.h"
#include "RSUtil.h"
#include "RCameraSceneNode.h"
#include "XMyPlayer.h"

#define SAY_NPC_COLOR		0xFFFFF799
#define MIN_PANEL_WIDTH		72

XOnScreenDialog::XOnScreenDialog( MUID uid, const char* szName, MWidget* pParent, MListener* pListener )
: MPanel(szName, pParent, pListener)
, m_UID(uid)
{
	_ASSERT(m_UID.IsValid());

	m_ShowRegulator.Stop();

//	SetBorderStyle(MBS_NONE);
	SetSize(BALLON_DEFAULTl_WIDTH, 100);

	m_pTextView = NULL;

	Create();
}

XOnScreenDialog::~XOnScreenDialog( void )
{
	m_UID.SetZero();
	m_ShowRegulator.Stop();

	//DeleteChildren();

	m_pLabel = NULL;
	m_pTextView = NULL;
}

void XOnScreenDialog::Create()
{
	if (m_pTextView != NULL) return;

	m_pTextView = new MTextView( NULL, this, this);
	Mint* mint = global.ui->GetMint();
// 	map<wstring, MBTextAreaLook*>::iterator itor =
// 		mint->GetLookManager()->m_TextAreaLookMap.find("look_BallonTextArea");
// 	m_pTextView->ChangeCustomLook((MBTextAreaLook*)(*itor).second);

//	MBTextAreaLook* pLook = (MBTextAreaLook*)(*itor).second;
// 	MBitmapRep* pBitmap = pLook->GetBitMap();
// 	m_WidthMrgin = pBitmap->m_rCoord[1].w;
// 	m_HeightMargin = pBitmap->m_rCoord[1].h + pBitmap->m_rCoord[8].h;		

	//m_pTextArea->SetOpacity(0.9f);
// 	m_pTextView->SetEditable(false);
// 	m_pTextView->SetFocusEnable(false);
// 	m_pTextView->SetScrollBarEnable(false);

	m_pTextView->SetRect( 0, 0, BALLON_DEFAULTl_WIDTH, 200);

	m_pLabel	= new MLabel(NULL, this, this);
//	MFont* pFont = MFontManager::Get("fnt_DefUI");
//	m_pLabel->SetFont(pFont);
}

// 폰트의 width, height 값이 정확하지 않은것 같아 3을 더해줌
int XOnScreenDialog::GetWidgetFontWidth( MWidget* pWidget, const wchar_t* str)
{
	if (pWidget == NULL) return 0;

//	MFont* pFont = pWidget->GetFont();
	MFont* pFont = MFontManager::Get( NULL);
	if (pFont == NULL) return 0;
	return pFont->GetWidth(MLocale::ConvUTF16ToAnsi(str).c_str()) + 3;
}

int XOnScreenDialog::GetWidgetFontHeight( MWidget* pWidget)
{
	if (pWidget == NULL) return 0;

//	MFont* pFont = pWidget->GetFont();
	MFont* pFont = MFontManager::Get( NULL);
	if (pFont == NULL) return 0;
	return pFont->GetHeight() + 3;
}

void XOnScreenDialog::CalculateSize( wstring szID, wstring szChat )
{
	int nIDWidth = GetWidgetFontWidth( m_pLabel, szID.c_str());
	int nChatWidth = GetWidgetFontWidth( m_pTextView, szChat.c_str());

	int width = BALLON_DEFAULTl_WIDTH - (m_WidthMrgin*2);
	if( nChatWidth > width )
	{
		nChatWidth = width;
	}
	else if (nChatWidth < MIN_PANEL_WIDTH)
	{
		nChatWidth = MIN_PANEL_WIDTH;
	}
	
	if (nChatWidth < nIDWidth)
	{
		nChatWidth = nIDWidth;
	}

	int nLine = (nChatWidth/width) + 1;

	// \n 계산
	int br_count = 0;
	wstring::size_type pos = 0;
	for ( ; pos < szChat.length() ; )
	{
		const wstring::size_type br = szChat.find('\n', pos);

		if (br == wstring::npos)
		{
			break;
		}

		pos = br+1;
		br_count++;
	}
	nLine += br_count;

	int nIDHeight = GetWidgetFontHeight(m_pLabel);
	m_pLabel->SetRect(m_WidthMrgin, 0, nIDWidth, nIDHeight);
//	m_pLabel->SetTextColor(SAY_NPC_COLOR);
	m_pLabel->SetText(MLocale::ConvUTF16ToAnsi(szID.c_str()).c_str());

	//이름이 말풍선 중간에 걸치도록 0.3f를 곱함
	int height = GetWidgetFontHeight(m_pTextView);
	m_pTextView->SetRect(0, nIDHeight*0.3f, nChatWidth+(m_WidthMrgin*2), (height*nLine)+m_HeightMargin);
	m_pTextView->SetText(MLocale::ConvUTF16ToAnsi(szChat.c_str()).c_str());

	SetSize(nChatWidth+(m_WidthMrgin*2), (nIDHeight)+(height*nLine)+m_HeightMargin);

	m_nChatWidth = nChatWidth;
	m_nLineCount = nLine;
}

void XOnScreenDialog::OnRun( void )
{
	m_Timer.Update();
	float fDelta = m_Timer.GetElapsedTime() * 0.001f;

	if(m_ShowRegulator.IsActive() == false) return;
	if(m_ShowRegulator.IsReady(fDelta))
	{
		m_ShowRegulator.Stop();

		// TODO : 보여줄 말들(vector)이 있으면 다시 온에어 호출 ?
		// 없으면 
		if (GetShow())	Show( false);
	}
	else
	{
		RCameraSceneNode *pCamera = global.smgr->GetPrimaryCamera();
		if (pCamera == NULL) return;

		if (gg.omgr == NULL) return;
		XObject* pObject = gg.omgr->FindObject(m_UID);
		if (pObject == NULL) return;

		// 카메라 뒤쪽에 있으면 hide 시키고 리턴
		vec3 vPos = pObject->GetPosition();
		vPos.z = 0.f;
		vec3 vCameraPos = pCamera->GetPosition();
		vCameraPos.z = 0.f;

		vec3 vMyPos = gvar.Game.pMyPlayer->GetPosition();
		vMyPos.z = 0.f;
		float fDist = vMyPos.DistanceTo(vPos);

		vec3 dir = (vPos - vCameraPos).Normalize();
		float fFront = pCamera->GetDirection().DotProduct(dir);
		if ( fDist > 2000.f || fFront < 0.f)
		{
			if (GetShow())	Show( false);
			return;
		}

		RVector2 pScreen(10,10);
		RVector vv = pObject->GetNamePos();

		RDevice *pDevice = REngine::GetDevicePtr();
		if (pDevice == NULL) return;

		rs3::GetScreenFromWorld(&pScreen, &vv
			, &pDevice->GetViewport(), &RMatrix::IDENTITY
			,&pCamera->GetViewMatrix(),&pCamera->GetProjectionMatrix());

		int nLineCount = m_nLineCount - 2;
		int height = GetWidgetFontHeight(m_pTextView);
		SetPosition(pScreen.x - (m_nChatWidth/2), pScreen.y-(height*nLineCount)-m_HeightMargin);

		if (!GetShow()) Show( true);
	}
}

void XOnScreenDialog::OnAir(wstring szID, wstring szChat, float fMaintainTime )
{
	SetInfo(szID, szChat);

	if (fMaintainTime == 0.f)
	{
		fMaintainTime = (max(10.f, szChat.length())) * 0.1f;
	}
	m_ShowRegulator.SetTickTime(fMaintainTime);
	m_ShowRegulator.Start();
}

void XOnScreenDialog::SetInfo( wstring szID, wstring szChat )
{
	if (m_pTextView == NULL)
	{
		Create();
	}

	CalculateSize(szID, szChat);
}
