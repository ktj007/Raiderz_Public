// CDlgCutSceneCaptionKey.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "CoreToolRS3.h"
#include "VD_DlgCutSceneCaptionKey.h"


// CDlgCutSceneCaptionKey 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgCutSceneCaptionKey, CDialog)

CDlgCutSceneCaptionKey::CDlgCutSceneCaptionKey(float _fStartTime, float _fDurationTime, CString& _strScript, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCutSceneCaptionKey::IDD, pParent)
{
	m_fStartTime = _fStartTime;
	m_fDurationTime = _fDurationTime;
	m_strScrip = _strScript;
}

CDlgCutSceneCaptionKey::~CDlgCutSceneCaptionKey()
{
}

void CDlgCutSceneCaptionKey::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_CAPTIONKEY_TIME, m_fStartTime);
	DDX_Text(pDX, IDC_EDIT_CAPTIONKEY_DURATIONTIME, m_fDurationTime);
	DDX_Text(pDX, IDC_EDIT_CAPTIONKEY_SCRIP, m_strScrip);
}


BEGIN_MESSAGE_MAP(CDlgCutSceneCaptionKey, CDialog)
END_MESSAGE_MAP()


// CDlgCutSceneCaptionKey 메시지 처리기입니다.

// CDlgCutSceneCaptionController 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgCutSceneCaptionController, CDialog)

CDlgCutSceneCaptionController::CDlgCutSceneCaptionController(const char* _pSzFont, int _nFontSize, BOOL _bBold, BOOL _bItalic, CWnd* pParent /*=NULL*/)
: CDialog(CDlgCutSceneCaptionController::IDD, pParent)
{
	if (_pSzFont)
	{
		m_strFont = _pSzFont;
	}
	else
	{
		m_strFont	= "굴림체";
	}

	m_nSize		= _nFontSize;
	m_bBold		= _bBold;
	m_bItalic	= _bItalic;
}

CDlgCutSceneCaptionController::~CDlgCutSceneCaptionController()
{
}

void CDlgCutSceneCaptionController::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_CAPTIONKEY_FONT, m_strFont);
	DDX_Text(pDX, IDC_EDIT_CAPTIONKEY_SIZE, m_nSize);
	DDX_Check(pDX, IDC_CHECK_BOLD, m_bBold);
	DDX_Check(pDX, IDC_CHECK_ITALIC, m_bItalic);
}


BEGIN_MESSAGE_MAP(CDlgCutSceneCaptionController, CDialog)
END_MESSAGE_MAP()


// CDlgCutSceneCaptionController 메시지 처리기입니다.