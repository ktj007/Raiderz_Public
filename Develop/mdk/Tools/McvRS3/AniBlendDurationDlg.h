#pragma once


// AniBlendDurationDlg 대화 상자입니다.

class AniBlendDurationDlg : public CDialog
{
	DECLARE_DYNAMIC(AniBlendDurationDlg)

public:
	AniBlendDurationDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~AniBlendDurationDlg();

	virtual void OnFinalRelease();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EDIT_ANIBLEND_DURATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	CString m_editDuration;
};
