#pragma once

#include <d3dx9.h>
//#include "RTypes.h"
#include "RDevice.h"
#include "afxwin.h"
// #include "REmitterXML.h"

// CPage2 대화 상자입니다.

class CPage2 : public CPPage
{
	DECLARE_DYNAMIC(CPage2)
public:
	void SendMessageBlend()
	{
		AfxGetMainWnd()->SendMessage( WM_MODIFY_MSG, 2);
	}
	void SendMessageTexture()
	{
		AfxGetMainWnd()->SendMessage( WM_MODIFY_MSG, 3, 0);
	}
	inline void SendMessageDumy()
	{
		AfxGetMainWnd()->SendMessage( WM_MODIFY_MSG, 9);
	}

public:
	CPage2();
	virtual ~CPage2();

	bool			ConfigFromXML();
	void			InitUIState();
	void			SetSrcBlendCtrl(rs3::RBLEND eBlend);
	void			SetDstBlendCtrl(rs3::RBLEND eBlend);
	void			DrawTextureToPicbox();
	void			CheckRadioButton();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_MATERIAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString		m_strPathname;
	CString		m_strCurrentPath;
	CString		m_strTextureName;

	CComboBox	m_ctlSrcBlend;
	CComboBox	m_ctlDstBlend;
	rs3::RBLEND m_eSrcBlend;
	rs3::RBLEND m_eDstBlend;

	CComboBox	m_ctlDumy;
	CString		m_strDumy;
	CListBox	m_ctlFiles;

	float		m_fTexU;
	float		m_fTexV;
	float		m_fTexX;
	float		m_fTexY;
	float		m_fAlphaRef;

	bool		m_bRadioOne;
	bool		m_bRadioTwo;
	bool		m_bRadioThree;

	virtual BOOL OnInitDialog();

	afx_msg void OnCbnSelchangeComboDumy();
	afx_msg void OnLbnSelchangeListFiles();
	afx_msg void OnPaint();
	afx_msg void OnEnChangeEditTex();
	afx_msg void OnBnClickedRadioOne();
	afx_msg void OnBnClickedRadioTwo();
	afx_msg void OnBnClickedRadioThree();
	afx_msg void OnCbnSelchangeComboSrcblend();
	afx_msg void OnCbnSelchangeComboDestblend();
	afx_msg void OnBnClickedButtonFolder();

public:
	afx_msg void OnEnChangeEditAlpharef();
};
