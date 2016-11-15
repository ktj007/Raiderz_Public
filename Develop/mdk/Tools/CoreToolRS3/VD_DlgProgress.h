#pragma once


// CDlgProgress 대화 상자입니다.

class CDlgProgress : public CDialog
{
	DECLARE_DYNAMIC(CDlgProgress)

public:
	CDlgProgress(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgProgress();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_PROGRESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_Progress;

	void SetRange(DWORD start, DWORD end);
	void SetStep(int value);
	void SetPos(DWORD pos);
	void StepIt();
	void ShowProgress();
};
