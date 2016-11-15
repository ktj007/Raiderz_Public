#pragma once
#include "afxwin.h"


// CLightSidebar 대화 상자입니다.

class CLightSidebar : public CDialog
{
	DECLARE_DYNAMIC(CLightSidebar)

public:
	CLightSidebar(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CLightSidebar();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_LIGHT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnOK() {}
	virtual void OnCancel() {}

	void PaintIcon( CWnd *pWnd, DWORD dwColor );
	void PaintIconDirLightDiffuse();
	void PaintIconDirLightAmbient();
	void RepaintAllIcon();
	bool IsButtonCheck( int controlID );

	void UpdateDirLightDiffuse( DWORD dwColor );
	void UpdateDirLightAmbient( DWORD dwColor );


	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();

	afx_msg void OnBnClickedCheckEnableSpecular();
	afx_msg void OnBnClickedCheckLightingOnly();
	afx_msg void OnBnClickedCheckDirlightMatchCamerapos();

	afx_msg void OnBnClickedCheckPointlight1Enable();
	afx_msg void OnBnClickedCheckPointlight2Enable();
	afx_msg void OnBnClickedCheckPointlight3Enable();

	afx_msg void OnStnClickedColorDirlightDiffuse();
	afx_msg void OnStnClickedColorDirlightAmbient();

protected:
	DWORD m_dwDirLightDiffuseR;
	DWORD m_dwDirLightDiffuseG;
	DWORD m_dwDirLightDiffuseB;

	DWORD m_dwDirLightAmbientR;
	DWORD m_dwDirLightAmbientG;
	DWORD m_dwDirLightAmbientB;
};
