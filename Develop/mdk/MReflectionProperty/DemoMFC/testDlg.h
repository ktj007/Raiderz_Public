
// testDlg.h : 헤더 파일
//

#pragma once

#include "..\MFCProperty\ReflectiveUI.h"
#include "..\MFCProperty\ReflectivePropertyGridCtrl.h"
#include "my.h"
#include <string>

// CtestDlg 대화 상자
class CtestDlg : public CDialog
{
// 생성입니다.
public:
	CtestDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.
	~CtestDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	void OnPropertyChanged();

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

//#########################################################################################
public:
	void InitPropGrid(void);
	CMFCPropertyGridProperty* createGridGroup(std::string groupName);
private:
	CMFCPropertyGridCtrl			m_wndPropList;
	CReflectiveUI					m_UI;
	REBox							m_box;
	RECircle						m_circle;

	// for multi objects
	REBox							m_multiBox1;
	REBox							m_multiBox2;

	CReflectivePropertyGridCtrl*	m_pPropCtrl;
	CReflectivePropertyGridCtrl*	m_pPropCtrl2;
	CReflectivePropertyGridCtrl*	m_pPropCtrl3;
};

