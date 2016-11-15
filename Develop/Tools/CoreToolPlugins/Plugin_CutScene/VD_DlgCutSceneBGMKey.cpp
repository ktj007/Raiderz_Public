// VD_DlgCutSceneBGMKey.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "CoreToolRS3.h"
#include "VD_DlgCutSceneBGMKey.h"

#include "VD_DlgInputCommon.h"


// CDlgCutSceneBGMKey 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgCutSceneBGMKey, CDialog)

CDlgCutSceneBGMKey::CDlgCutSceneBGMKey(float _fTime, const char* _pSzFileName, float _fVolume, bool _bLoop, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCutSceneBGMKey::IDD, pParent)
{
	CDlgInputCommon::m_isActive = true;

	m_fTime = _fTime;
	if (_pSzFileName)
	{
		m_strFileName = _pSzFileName;
	}
	
	m_fVolume = _fVolume;
	m_bLoop = _bLoop? TRUE : FALSE;;
}

CDlgCutSceneBGMKey::~CDlgCutSceneBGMKey()
{
	CDlgInputCommon::m_isActive = false;
}

void CDlgCutSceneBGMKey::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_BGMKEY_TIME, m_fTime);
	DDX_Text(pDX, IDC_EDIT_BGMKEY_FILENAME, m_strFileName);

	DDX_Text(pDX, IDC_EDIT_BGMKEY_VOLUME, m_fVolume);
	DDX_Check(pDX, IDC_CHECK_BGMKEY_LOOP, m_bLoop);
}


BEGIN_MESSAGE_MAP(CDlgCutSceneBGMKey, CDialog)
END_MESSAGE_MAP()


// CDlgCutSceneBGMKey 메시지 처리기입니다.
