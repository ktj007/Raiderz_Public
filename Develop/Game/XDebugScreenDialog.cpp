#include "StdAfx.h"
#include "XDebugScreenDialog.h"
#include "MMainFrame.h"
#include "RSUtil.h"
#include "RCameraSceneNode.h"
#include "XCharacter.h"

using namespace rs3;

XDebugScreenDialog::XDebugScreenDialog(  XObject* pOwner, const char* szName, MWidget* pParent, MListener* pListener )
: MPanel(szName, pParent, pListener)
, m_pOwner(pOwner)
{
	_ASSERT(m_pOwner);

//	SetBorderStyle(MBS_NONE);
	SetSize(BALLON_DEFAULTl_WIDTH, 100);

	m_pLabel	= new MLabel(NULL, this, this);
//	m_pTextArea = new MTextArea(BALLON_DEFAULTl_WIDTH, "OnScreenTextArea", this, this);
//	Mint* mint = global.ui->GetMint();
//	map<wstring, MBTextAreaLook*>::iterator itor =
//		mint->GetLookManager()->m_TextAreaLookMap.find("look_DefaultTextArea");
//	m_pTextArea->ChangeCustomLook((MBTextAreaLook*)(*itor).second);

// 	m_pTextArea->SetOpacity(0.9f);
// 	m_pTextArea->SetEditable(false);
// 	m_pTextArea->SetFocusEnable(false);
// 	m_pTextArea->SetScrollBarEnable(false);
}

XDebugScreenDialog::~XDebugScreenDialog( void )
{

}

void XDebugScreenDialog::OnAir(wstring szID, wstring szChat)
{
	if (m_pOwner == NULL) return;

	if(CheckVisible())
	{
		Hide();
		return;
	}

	SetInfo(szID, szChat);
}

void XDebugScreenDialog::SetInfo( wstring szID, wstring szChat )
{
	m_nChatSize = GetLineWidth(szChat.c_str()) + 12;
	//int nLine = (m_nChatSize/108)+1;
	if( m_nChatSize > 108 )
	{
		m_nChatSize = 108;
	}

	int nIDSize = GetLineWidth(szID.c_str()) + 12;
	if ( m_nChatSize < nIDSize)
	{
		m_nChatSize = nIDSize;
	}

	Init(szID, szChat);
	Show();
}

void XDebugScreenDialog::Init( wstring szID, wstring szChat )
{
	int nWidth = GetLineWidth(szChat.c_str()) + 12;
	int nLine = (nWidth/108)+1;
	if( nWidth > 108 )
	{
		nWidth = 108;
	}

	int nIDWidth= GetLineWidth(szID.c_str()) + 12;
	if (nWidth < nIDWidth)
	{
		nWidth = nIDWidth;
	}

	nWidth *= 2;
	nLine = 12;

//	int nIDHeight = m_pLabel->GetFont()->GetHeight()+3;
	int nIDHeight = MFontManager::Get( NULL)->GetHeight()+3;
	m_pLabel->SetRect(0, 0, nWidth, nIDHeight);
	m_pLabel->SetText(MLocale::ConvUTF16ToAnsi(szID.c_str()).c_str());

//	int height = m_pTextArea->GetFont()->GetHeight()+3;
//	m_pTextArea->SetBounds(0, nIDHeight, nWidth, height*nLine);
//	m_pTextArea->SetText(szChat.c_str());

	int height = MFontManager::Get( NULL)->GetHeight()+3;

	SetSize(nWidth, nIDHeight+height*nLine);
}

bool XDebugScreenDialog::CheckVisible()
{
	//Actor
	if(m_pOwner->GetActor()->GetVisible() == false ||
		m_pOwner->GetActor()->IsPrimaryCamRenerable() == false)
		return true;

	//°Å¸®
	RCameraSceneNode* pCam = REngine::GetSceneManagerPtr()->GetPrimaryCamera();
	if( pCam == NULL )
		return false;

	RVector vCamPos(pCam->GetPosition());
	float fDist = vCamPos.DistanceTo( m_pOwner->GetPosition() );

	if(fDist > SHOW_NAME_DISTANCE)
		return true;

	return false;
}

void XDebugScreenDialog::OnRun( void )
{
	if (m_pOwner == NULL) return;

	RVector2 pScreen(10,10);

	RVector vv = m_pOwner->GetNamePos();
	vv.z = min(vv.z, 300.f);

	RCameraSceneNode *pCamera = global.smgr->GetPrimaryCamera();
	RDevice *pDevice = REngine::GetDevicePtr();

	rs3::GetScreenFromWorld(&pScreen, &vv, &pDevice->GetViewport(), &RMatrix::IDENTITY
		,&pCamera->GetViewMatrix(),&pCamera->GetProjectionMatrix());

	SetPosition(pScreen.x + (m_nChatSize/2), pScreen.y+12);
}