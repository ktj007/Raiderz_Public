#pragma once


// CDlgCutSceneBGMKey 대화 상자입니다.

class CDlgCutSceneBGMKey : public CDialog
{
	DECLARE_DYNAMIC(CDlgCutSceneBGMKey)

public:
	CDlgCutSceneBGMKey(float _fTime, const char* _pSzFileName, float _fVolume, bool _bLoop, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgCutSceneBGMKey();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_CUTSCENE_BGMKEY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	float	m_fTime;
	CString m_strFileName;
	float	m_fVolume;
	BOOL	m_bLoop;
};
