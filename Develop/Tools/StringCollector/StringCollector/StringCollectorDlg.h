
// StringCollectorDlg.h : 헤더 파일
//

#pragma once


// CStringCollectorDlg 대화 상자
class CStringCollectorDlg : public CDialog
{
// 생성입니다.
public:
	CStringCollectorDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_STRINGCOLLECTOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;


	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedSearch();
	afx_msg void OnBnClickedSearch2();
	afx_msg void OnBnClickedStart();

	DECLARE_MESSAGE_MAP()
};
