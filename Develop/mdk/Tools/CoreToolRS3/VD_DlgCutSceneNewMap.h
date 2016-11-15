#pragma once


// CDlgCutSceneNewMap 대화 상자입니다.

class CORETOOL_API CDlgCutSceneNewMap : public CDialog
{
	DECLARE_DYNAMIC(CDlgCutSceneNewMap)

public:
	CDlgCutSceneNewMap(CControlMediator* pControlMediator, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgCutSceneNewMap();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_CUTSCENE_NEWMAP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

private:
	CControlMediator* m_pControlMediator_;

public:
	CString		m_strFileName;
	CString		m_strName;

	int			m_nFieldId;
	float		m_fDuration;

	bool		MakeNewCutSceneFile();

	afx_msg void OnBnClickedButtonCutsceneFieldOpen();
};
