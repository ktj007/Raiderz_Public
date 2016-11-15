#pragma once


// CDlgCutSceneCaptionKey 대화 상자입니다.

class CDlgCutSceneCaptionKey : public CDialog
{
	DECLARE_DYNAMIC(CDlgCutSceneCaptionKey)

public:
	CDlgCutSceneCaptionKey(float _fStartTime, float _fDurationTime, CString& _strScript, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgCutSceneCaptionKey();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_CUTSCENE_CAPTIONKEY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	float	m_fStartTime;
	float	m_fDurationTime;
	CString m_strScrip;
};

// CDlgCutSceneCaptionController 대화 상자입니다.

class CDlgCutSceneCaptionController : public CDialog
{
	DECLARE_DYNAMIC(CDlgCutSceneCaptionController)

public:
	CDlgCutSceneCaptionController(const char* _pSzFont, int _nFontSize, BOOL _bBold, BOOL _bItalic, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgCutSceneCaptionController();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_CUTSCENE_CAPTION_ATTRIBUTE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	CString m_strFont;
	int		m_nSize;
	BOOL	m_bBold;
	BOOL	m_bItalic;
};
