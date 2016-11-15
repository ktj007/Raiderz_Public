// VD_DlgCutSceneAniKey.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "CoreToolRS3.h"
#include "VD_DlgCutSceneAniKey.h"

#include "VD_DlgInputCommon.h"


// CDlgCutSceneAniKey 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgCutSceneAniKey, CDialog)

CDlgCutSceneAniKey::CDlgCutSceneAniKey(float _fTime, const char* _pSzStrAnimation, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCutSceneAniKey::IDD, pParent)
{
	CDlgInputCommon::m_isActive = true;

	m_fTime = _fTime;
	if (_pSzStrAnimation)
		m_strAnimation = _pSzStrAnimation;
}

CDlgCutSceneAniKey::~CDlgCutSceneAniKey()
{
	CDlgInputCommon::m_isActive = false;
}

void CDlgCutSceneAniKey::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_ANIKEY_TIME, m_fTime);
	DDX_Text(pDX, IDC_EDIT_ANIKEY_NAME, m_strAnimation);
}


BEGIN_MESSAGE_MAP(CDlgCutSceneAniKey, CDialog)
END_MESSAGE_MAP()


// CDlgCutSceneAniKey 메시지 처리기입니다.
