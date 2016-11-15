#pragma once


// CDlgInputBool 대화 상자입니다.

class CORETOOL_API CDlgInputBool : public CDialog
{
	DECLARE_DYNAMIC(CDlgInputBool)

public:
	CDlgInputBool(const char* _pSzWindowName, int _nTrueChecked, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgInputBool();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_INPUT_BOOL };
	int m_nBoolValue;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

private:
	CString m_cstrWindowName_;
};
