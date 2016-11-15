#pragma once


// CDlgCutSceneDOFKey 대화 상자입니다.

class CDlgCutSceneDOFKey : public CDialog
{
	DECLARE_DYNAMIC(CDlgCutSceneDOFKey)

public:
	CDlgCutSceneDOFKey(float _fTime, float _fFocusDist, float _fFocusRange, float _fCocSize, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgCutSceneDOFKey();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_CUTSCENE_DOFKEY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	float m_fTime;
	float m_fFocusDist;
	float m_fFocusRange;
	float m_fCocSize;
};
