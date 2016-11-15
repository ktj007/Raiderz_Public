#pragma once


class CControlMediator;
// CDlgCutSceneActorType 대화 상자입니다.

class CDlgCutSceneActorType : public CDialog
{
	DECLARE_DYNAMIC(CDlgCutSceneActorType)

public:
	CDlgCutSceneActorType(CControlMediator* pControlMediator, const char* _pSzActorType, const char* _pSzActorName, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgCutSceneActorType();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_CUTSCENE_ACTOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	CControlMediator* m_pControlMediator_;

public:
	int m_nActorType;
	int m_nNPCID;
	CString m_strNpcName;

	void SetEnablePlayer();
	void SetEnableNPC();
	afx_msg void OnBnClickedRadioCutscenePlayer();
	afx_msg void OnBnClickedRadioCutsceneNpc();
	afx_msg void OnBnClickedButtonCutsceneNpcSelect();
};
