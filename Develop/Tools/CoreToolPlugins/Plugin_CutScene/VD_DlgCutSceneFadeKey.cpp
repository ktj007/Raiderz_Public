// VD_DlgCutSceneEffectKey.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "CoreToolRS3.h"

#include "C_ControlMediator.h"
#include "VD_DlgCutSceneFadeKey.h"
#include "VR_CutSceneObjectList.h"
#include "VR_EntityObjectList.h"

#include "../../Game/GameCommon/XEffectInfo.h"

#pragma warning( push )
#pragma warning( disable : 4996 )

// CDlgCutSceneFadeKey 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgCutSceneFadeKey, CDialog)

CDlgCutSceneFadeKey::CDlgCutSceneFadeKey(CControlMediator* _pControlMediator, float fTime, float fAlpha, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCutSceneFadeKey::IDD, pParent)
{
	m_fTime			= fTime;
	m_fAlpha		= fAlpha;
}

CDlgCutSceneFadeKey::~CDlgCutSceneFadeKey()
{
}

void CDlgCutSceneFadeKey::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_FADEKEY_TIME, m_fTime);
	DDX_Text(pDX, IDC_EDIT_FADEKEY_ALPHA, m_fAlpha);
}

void CDlgCutSceneFadeKey::GetFadeData( float& fTime, float& fAlpha)
{
	fTime	= m_fTime;
	fAlpha	= m_fAlpha;
}

BEGIN_MESSAGE_MAP(CDlgCutSceneFadeKey, CDialog)
END_MESSAGE_MAP()

#pragma warning( pop )