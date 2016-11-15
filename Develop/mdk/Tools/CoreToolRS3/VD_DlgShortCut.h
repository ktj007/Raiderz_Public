/// @file VD_DlgShortCut.h
#pragma once

// CDlgShortCut 대화 상자입니다.
/**
	@brief 단축키 정보

*/
class CDlgShortCut : public CDialog
{

public:
	CDlgShortCut();   // 표준 생성자입니다.
	virtual ~CDlgShortCut();
	virtual void OnOK();

	virtual BOOL OnInitDialog();	///< 생성시에 필요한 font등의 생성

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SHORTCUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

private:
	CString m_strShortcutText;
};
