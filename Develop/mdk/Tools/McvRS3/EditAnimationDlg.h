#pragma once
#include "afxwin.h"


// EditAnimationDlg 대화 상자입니다.

class EditAnimationDlg : public CDialog
{
	DECLARE_DYNAMIC(EditAnimationDlg)

public:
	EditAnimationDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~EditAnimationDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EDIT_ANIMATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strName;
	CString m_strFileName;
	CComboBox m_cbLoopType;
	BOOL m_bLoop;
};
