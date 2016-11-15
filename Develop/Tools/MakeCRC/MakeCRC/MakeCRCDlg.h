
// MakeCRCDlg.h : 헤더 파일
//

#pragma once


// CMakeCRCDlg 대화 상자
class CMakeCRCDlg : public CDialog
{
// 생성입니다.
public:
	CMakeCRCDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_MAKECRC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

	void Run( const char* szPath);


// 구현입니다.
protected:
	HICON m_hIcon;
	CString m_strPath;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedStart();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()
};
