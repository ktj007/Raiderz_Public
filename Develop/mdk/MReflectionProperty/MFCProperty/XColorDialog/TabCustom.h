// TabCustom.h  Version 1.0 - see article at CodeProject.com
//
// Author:  Hans Dietrich
//          hdietrich@gmail.com
//
// License:
//     This software is released under the Code Project Open License (CPOL),
//     which may be found here:  http://www.codeproject.com/info/eula.aspx
//     You are free to use this software in any way you like, except that you 
//     may not sell this source code.
//
//     This software is provided "as is" with no expressed or implied warranty.
//     I accept no liability for any damage or loss of business that this 
//     software may cause.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef TABCUSTOM_H
#define TABCUSTOM_H

#include "XColorSpectrumCtrl.h"

//=============================================================================
class CTabCustom : public CDialog
//=============================================================================
{
// Construction
public:
	CTabCustom(COLORREF crCurrent, 
			   COLORREF crStartColor, 
			   COLORREF crEndColor, 
			   CXColorSpectrumCtrl::TOOLTIP_FORMAT eFormat, 
			   CWnd* pParent,
			   HICON hHelpIcon);

	~CTabCustom();

// Dialog Data
	//{{AFX_DATA(CTabCustom)
	CEdit			m_editRed;
	CSpinButtonCtrl	m_spinRed;
	CSliderCtrl		m_sliderRed;
	CEdit			m_editGreen;
	CSpinButtonCtrl	m_spinGreen;
	CSliderCtrl		m_sliderGreen;
	CEdit			m_editBlue;
	CSpinButtonCtrl	m_spinBlue;
	CSliderCtrl		m_sliderBlue;
	CEdit			m_editHue;
	CSpinButtonCtrl	m_spinHue;
	CSliderCtrl		m_sliderHue;
	CEdit			m_editSat;
	CSpinButtonCtrl	m_spinSat;
	CSliderCtrl		m_sliderSat;
	CEdit			m_editLum;
	CSpinButtonCtrl	m_spinLum;
	CSliderCtrl		m_sliderLum;
	UINT			m_nRed;
	UINT			m_nGreen;
	UINT			m_nBlue;
	UINT			m_nHue;
	UINT			m_nSat;
	UINT			m_nLum;
	//}}AFX_DATA

	// Attributes
public:
	CXColorSpectrumCtrl m_ColorSpectrum;

	int GetColorModel();
	void GetHSL(BYTE* h, BYTE* s, BYTE* l)
	{
		*h = (BYTE)m_nHue;
		*s = (BYTE)m_nSat;
		*l = (BYTE)m_nLum;
	}
	COLORREF GetRGB() { return RGB(m_nRed, m_nGreen, m_nBlue); }
	void SetHSL(BYTE h, BYTE s, BYTE l);
	void SetRGB(COLORREF cr);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabCustom)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON		m_hHelpIcon;
	CWnd		*m_pParent;
	COLORREF	m_crCurrent;
	COLORREF	m_crNew;
	COLORREF	m_crStartColor;
	COLORREF	m_crEndColor;
	CXColorSpectrumCtrl::TOOLTIP_FORMAT m_eFormat;
	HACCEL		m_hAccel;
	BOOL		m_bInitDone;
	CBrush		m_brushNew;
	CBrush		m_brushCurrent;

	void SetValues();

	void UpdateByRGB();
	void UpdateByHSL();

	void OnCancel() override;

	// Generated message map functions
	//{{AFX_MSG(CTabCustom)
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnChangeEditRGB();
	afx_msg void OnChangeEditHSL();
	afx_msg void OnHScroll(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar);
	//}}AFX_MSG
	afx_msg LRESULT OnSelChange(WPARAM, LPARAM);
	afx_msg LRESULT OnSelendOk(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //TABCUSTOM_H
