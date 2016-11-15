#pragma once


// CDlgCutSceneMoveKey 대화 상자입니다.

class CDlgCutSceneMoveKey : public CDialog
{
	DECLARE_DYNAMIC(CDlgCutSceneMoveKey)

public:
	CDlgCutSceneMoveKey(float _fTime, const char* _pSzPos, const char* _pSzDir, bool _bIsWarp = false, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgCutSceneMoveKey();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_CUTSCENE_MOVEKEY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	float	m_fTime;
	CString	m_strPos;
	CString	m_strDir;
	BOOL	m_bWarpLocation;
};
