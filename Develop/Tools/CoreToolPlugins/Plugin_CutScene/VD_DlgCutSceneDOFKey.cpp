// VD_DlgCutSceneDOFKey.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "CoreToolRS3.h"
#include "VD_DlgCutSceneDOFKey.h"


// CDlgCutSceneDOFKey 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgCutSceneDOFKey, CDialog)

CDlgCutSceneDOFKey::CDlgCutSceneDOFKey(float _fTime, float _fFocusDist, float _fFocusRange, float _fCocSize, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCutSceneDOFKey::IDD, pParent)
{
	m_fTime = _fTime;
	m_fFocusDist = _fFocusDist;
	m_fFocusRange = _fFocusRange;
	m_fCocSize = _fCocSize;
}

CDlgCutSceneDOFKey::~CDlgCutSceneDOFKey()
{
}

void CDlgCutSceneDOFKey::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_DOFKEY_TIME	, m_fTime);
	DDX_Text(pDX, IDC_EDIT_DOFKEY_FOCUSDIST, m_fFocusDist);
	DDX_Text(pDX, IDC_EDIT_DOFKEY_FOCUSRANGE, m_fFocusRange);
	DDX_Text(pDX, IDC_EDIT_DOFKEY_COCSIZE, m_fCocSize);
}


BEGIN_MESSAGE_MAP(CDlgCutSceneDOFKey, CDialog)
END_MESSAGE_MAP()


// CDlgCutSceneDOFKey 메시지 처리기입니다.
