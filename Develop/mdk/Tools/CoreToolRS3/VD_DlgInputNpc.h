#pragma once


class CControlMediator;
class CORETOOL_API CDlgInputNpc : public CDialog
{
	DECLARE_DYNAMIC(CDlgInputNpc)

public:
	CDlgInputNpc(CControlMediator* _pControlMediator,
		int _nID, const char* _pSzWindowName, CWnd* pParent = NULL);
	virtual ~CDlgInputNpc();

	int m_nNpcID;

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_INPUT_NPC };
	static bool m_isActive;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	afx_msg void OnBnClickedButtonOpenNpcMgr();
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

private:
	CControlMediator* m_pControlMediator_;
	CString m_cstrWindowName_;
};
