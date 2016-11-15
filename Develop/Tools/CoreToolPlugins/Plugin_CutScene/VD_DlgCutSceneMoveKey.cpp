// VD_DlgCutSceneMoveKey.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "CoreToolRS3.h"
#include "VD_DlgCutSceneMoveKey.h"


// CDlgCutSceneMoveKey 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgCutSceneMoveKey, CDialog)

CDlgCutSceneMoveKey::CDlgCutSceneMoveKey(float _fTime, const char* _pSzPos, const char* _pSzDir, bool _bIsWarp, CWnd* pParent /*=NULL*/)
: CDialog(CDlgCutSceneMoveKey::IDD, pParent)
{
	m_fTime = _fTime;

	if (_pSzPos)
		m_strPos = _pSzPos;

	if (_pSzDir)
		m_strDir = _pSzDir;

	m_bWarpLocation = _bIsWarp? TRUE : FALSE;
}

CDlgCutSceneMoveKey::~CDlgCutSceneMoveKey()
{
}

void CDlgCutSceneMoveKey::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_CUTSCENEKEY_TIME	, m_fTime);
	DDX_Text(pDX, IDC_EDIT_CUTSCENE_POS, m_strPos);
	DDX_Text(pDX, IDC_EDIT_CUTSCENE_DIR, m_strDir);

	DDX_Check(pDX, IDC_CHECK_CUTSCENEKEY_WARP, m_bWarpLocation);
}

BEGIN_MESSAGE_MAP(CDlgCutSceneMoveKey, CDialog)
END_MESSAGE_MAP()

// CDlgCutSceneMoveKey 메시지 처리기입니다.
