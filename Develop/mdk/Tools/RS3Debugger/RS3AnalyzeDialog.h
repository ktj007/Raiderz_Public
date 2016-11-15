
// RS3DebuggerDlg.h : 헤더 파일
//

#pragma once

#include "RDebuggingInfo.h"
#include "RCPUAnalyzeDialog.h"
#include "RResourceAnalyzeDialog.h"


// CRS3AnalyzeDialoga 대화 상자
class CRS3AnalyzeDialoga : public CDialog
{
// 생성입니다.
public:
	CRS3AnalyzeDialoga(CWnd* pParent = NULL);	// 표준 생성자입니다.
	virtual ~CRS3AnalyzeDialoga(void);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_RS3DEBUGGER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

private:
	RCPUAnalyzeDialog*				m_pProfileDialog;
	RResourceAnalyzeDialog*	m_pPerformanceViewerDialog;

public:
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg void OnBnClickedButton1();	//PerformanceViewer버튼
	afx_msg void OnBnClickedButton2();	//Profile버튼

private:
	void	PerformanceViewerDialogOnOff(void);
	
};
