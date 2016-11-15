// TabCustom.cpp  Version 1.0 - see article at CodeProject.com
//
// Author:  Hans Dietrich
//          hdietrich@gmail.com
//
// Description:
//     TabCustom implements CTabCustom, the Custom page that mimics the
//     functionality of the "Custom Colors" color picker in MS Office.
//
// History
//     Version 1.0 - 2008 April 5
//     - Initial public release
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

#include "stdafx.h"
#include "XColorDialogRes.h"
#include "TabCustom.h"
#include "XBalloonMsg.h"

#pragma warning(disable : 4996)	// disable bogus deprecation warning

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef __noop
#if _MSC_VER < 1300
#define __noop ((void)0)
#endif
#endif

#undef TRACE
#define TRACE __noop
#undef TRACERECT
#define TRACERECT __noop

//=============================================================================
// if you want to see the TRACE output, uncomment this line:
//#include "XTrace.h"

static const int g_nCtrlId = IDC_SPECTRUM;

//=============================================================================
void XDDX_Text(CDataExchange* pDX, int nID, UINT& value)
//=============================================================================
{
	TCHAR szText[50];
	TCHAR szNewText[50];
	HWND hWndCtrl = pDX->PrepareEditCtrl(nID);
	::GetWindowText(hWndCtrl, szText, sizeof(szText)/sizeof(TCHAR)-1);

	if (pDX->m_bSaveAndValidate)
	{
		// moving to data variables
		size_t len = _tcsspn(szText, _T("0123456789"));
		if (len != _tcslen(szText))
		{
			// there is non-digit char in string
			int j = 0;
			for (UINT i = 0; i < _tcslen(szText); i++)
			{
				// remove non-digit chars
				if (_istdigit(szText[i]))
					szNewText[j++] = szText[i];
			}
			szNewText[j++] = _T('\0');
			::SetWindowText(hWndCtrl, szNewText);
			value = _ttoi(szNewText);
			CXBalloonMsg::Show(LPCTSTR_DEFAULT_TITLE,
					 _T("Please enter only numeric characters."), 
					 pDX->m_pDlgWnd->GetDlgItem(nID)->m_hWnd, 
					 pDX->m_pDlgWnd->m_hWnd,
					 AfxGetInstanceHandle(),
					 TTI_WARNING);
			pDX->Fail();
		}
		else
		{
			value = _ttoi(szText);
		}
	}
	else
	{
		// moving to control
		_sntprintf(szNewText, sizeof(szNewText)/sizeof(TCHAR)-1, _T("%u"), value);
		if (_tcscmp(szText, szNewText) != 0)
		{
			::SetWindowText(hWndCtrl, szNewText);
		}
	}
}

//=============================================================================
void XDDV_MinMaxUInt(CDataExchange* pDX, UINT nID, UINT& value, UINT minVal, UINT maxVal)
//=============================================================================
{
	TRACE(_T("in XDDV_MinMaxUInt:  value=%d\n"), value);
	ASSERT(minVal <= maxVal);

	HWND hWndCtrl = pDX->PrepareEditCtrl(nID);

	if (value < minVal || value > maxVal)
	{
		if (!pDX->m_bSaveAndValidate)
		{
			TRACE(_T("Warning: initial dialog data is out of range.\n"));
		}
		else
		{
			CString prompt = _T("");
			prompt.Format(_T("Please enter an integer between %d and %d."),
				minVal, maxVal);

			CXBalloonMsg::Show(LPCTSTR_DEFAULT_TITLE,
					 prompt, 
					 pDX->m_pDlgWnd->GetDlgItem(nID)->m_hWnd, 
					 ::GetParent(pDX->m_pDlgWnd->m_hWnd),
					 AfxGetInstanceHandle(),
					 TTI_WARNING);
			
			if (pDX->m_pDlgWnd && ::IsWindow(pDX->m_pDlgWnd->m_hWnd))
			{
				TCHAR szNewText[50];
				_sntprintf(szNewText, sizeof(szNewText)/sizeof(TCHAR)-1, _T("%u"), maxVal);
				::SetWindowText(hWndCtrl, szNewText);
				value = maxVal;
			}
			pDX->Fail();
		}
	}
}

//=============================================================================
BEGIN_MESSAGE_MAP(CTabCustom, CDialog)
//=============================================================================
	//{{AFX_MSG_MAP(CTabCustom)
	ON_WM_CTLCOLOR()
	ON_WM_HELPINFO()
	ON_EN_CHANGE(IDC_RED_EDIT, OnChangeEditRGB)
	ON_EN_CHANGE(IDC_GREEN_EDIT, OnChangeEditRGB)
	ON_EN_CHANGE(IDC_BLUE_EDIT, OnChangeEditRGB)
	ON_EN_CHANGE(IDC_HUE_EDIT, OnChangeEditHSL)
	ON_EN_CHANGE(IDC_SAT_EDIT, OnChangeEditHSL)
	ON_EN_CHANGE(IDC_LUM_EDIT, OnChangeEditHSL)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(WM_XCOLORPICKER_SELCHANGE, OnSelChange)
	ON_REGISTERED_MESSAGE(WM_XCOLORPICKER_SELENDOK, OnSelendOk)
END_MESSAGE_MAP()

//=============================================================================
CTabCustom::CTabCustom(
//=============================================================================
					   COLORREF crCurrent, 
					   COLORREF crStartColor, 
					   COLORREF crEndColor, 
					   CXColorSpectrumCtrl::TOOLTIP_FORMAT eFormat, 
					   CWnd* pParent,
					   HICON hHelpIcon)
  : CDialog(_T("IDD_TAB_CUSTOM"), NULL),
	m_crCurrent(crCurrent),
	m_crNew(crCurrent),
	m_crStartColor(crStartColor),
	m_crEndColor(crEndColor),
	m_eFormat(eFormat),
	m_pParent(pParent),
	m_hHelpIcon(hHelpIcon),
	m_bInitDone(FALSE),
	m_hAccel(0)
{
	//{{AFX_DATA_INIT(CTabCustom)
	m_nRed = m_nGreen = m_nBlue = 0;
	m_nHue = m_nSat = m_nLum = 0;
	//}}AFX_DATA_INIT

	m_brushNew.CreateSolidBrush(m_crNew);
	m_brushCurrent.CreateSolidBrush(m_crCurrent);

	ASSERT(m_pParent);
}

//=============================================================================
CTabCustom::~CTabCustom()
//=============================================================================
{
	if (m_hAccel)
		::DestroyAcceleratorTable(m_hAccel);
	m_hAccel = 0;
}

//=============================================================================
void CTabCustom::DoDataExchange(CDataExchange* pDX)
//=============================================================================
{
	TRACE(_T("in CTabCustom::DoDataExchange\n"));
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabCustom)
	DDX_Control(pDX,   IDC_RED_EDIT,    m_editRed);
	DDX_Control(pDX,   IDC_RED_SPIN,    m_spinRed);
	DDX_Control(pDX,   IDC_RED_SLIDER,	m_sliderRed);
	DDX_Control(pDX,   IDC_GREEN_EDIT,  m_editGreen);
	DDX_Control(pDX,   IDC_GREEN_SPIN,  m_spinGreen);
	DDX_Control(pDX,   IDC_GREEN_SLIDER,m_sliderGreen);
	DDX_Control(pDX,   IDC_BLUE_EDIT,   m_editBlue);
	DDX_Control(pDX,   IDC_BLUE_SPIN,   m_spinBlue);
	DDX_Control(pDX,   IDC_BLUE_SLIDER,	m_sliderBlue);
	DDX_Control(pDX,   IDC_HUE_EDIT,    m_editHue);
	DDX_Control(pDX,   IDC_HUE_SPIN,    m_spinHue);
	DDX_Control(pDX,   IDC_HUE_SLIDER,	m_sliderHue);
	DDX_Control(pDX,   IDC_SAT_EDIT,    m_editSat);
	DDX_Control(pDX,   IDC_SAT_SPIN,    m_spinSat);
	DDX_Control(pDX,   IDC_SAT_SLIDER,	m_sliderSat);
	DDX_Control(pDX,   IDC_LUM_EDIT,    m_editLum);
	DDX_Control(pDX,   IDC_LUM_SPIN,    m_spinLum);
	DDX_Control(pDX,   IDC_LUM_SLIDER,	m_sliderLum);

	XDDX_Text(pDX,     IDC_RED_EDIT,    m_nRed);
	XDDX_Text(pDX,     IDC_GREEN_EDIT,  m_nGreen);
	XDDX_Text(pDX,     IDC_BLUE_EDIT,   m_nBlue);
	XDDX_Text(pDX,     IDC_HUE_EDIT,    m_nHue);
	XDDX_Text(pDX,     IDC_SAT_EDIT,    m_nSat);
	XDDX_Text(pDX,     IDC_LUM_EDIT,    m_nLum);
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate)
	{
		TRACE(_T("moving data to data members\n"));
		XDDV_MinMaxUInt(pDX, IDC_RED_EDIT,   m_nRed,   0, 255);
		XDDV_MinMaxUInt(pDX, IDC_GREEN_EDIT, m_nGreen, 0, 255);
		XDDV_MinMaxUInt(pDX, IDC_BLUE_EDIT,  m_nBlue,  0, 255);
		XDDV_MinMaxUInt(pDX, IDC_HUE_EDIT,   m_nHue,   0, 239);
		XDDV_MinMaxUInt(pDX, IDC_SAT_EDIT,   m_nSat,   0, 240);
		XDDV_MinMaxUInt(pDX, IDC_LUM_EDIT,   m_nLum,   0, 240);

	}

	m_sliderRed.SetPos( m_nRed );
	m_sliderGreen.SetPos( m_nGreen );
	m_sliderBlue.SetPos( m_nBlue);
	m_sliderHue.SetPos( m_nHue );
	m_sliderSat.SetPos( m_nSat );
	m_sliderLum.SetPos( m_nLum );
}

//=============================================================================
BOOL CTabCustom::OnInitDialog() 
//=============================================================================
{
	TRACE(_T("in CTabCustom::OnInitDialog\n"));

	CDialog::OnInitDialog();

	const int THUMB_SIZE = 20;

#define SETUP_SLIDER(_ctrl, _min,_max) { 	_ctrl.SetRange(_min, _max); \
											_ctrl.ModifyStyle(0,TBS_FIXEDLENGTH,FALSE);	\
											_ctrl.SetThumbLength(THUMB_SIZE);	}

	m_editRed.LimitText(3);
	m_spinRed.SetRange(0, 255);
	SETUP_SLIDER(m_sliderRed, 0, 255);
	m_editGreen.LimitText(3);
	m_spinGreen.SetRange(0, 255);
	SETUP_SLIDER(m_sliderGreen, 0, 255);
	m_editBlue.LimitText(3);
	m_spinBlue.SetRange(0, 255);
	SETUP_SLIDER(m_sliderBlue, 0, 255);
	m_editHue.LimitText(3);
	m_spinHue.SetRange(0, 239);
	SETUP_SLIDER(m_sliderHue, 0, 239);
	m_editSat.LimitText(3);
	m_spinSat.SetRange(0, 240);
	SETUP_SLIDER(m_sliderSat, 0, 240);
	m_editLum.LimitText(3);
	m_spinLum.SetRange(0, 240);
	SETUP_SLIDER(m_sliderLum, 0, 240);

	CRect rect;
	GetDlgItem(IDC_FRAME)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	GetDlgItem(IDC_FRAME)->ShowWindow(SW_HIDE);
	VERIFY(m_ColorSpectrum.Create(AfxGetInstanceHandle(), WS_CHILD | WS_VISIBLE | WS_TABSTOP /*| WS_BORDER*/, 
		rect, m_hWnd, g_nCtrlId, m_crCurrent));

	// call SetWindowPos to insert control in proper place in tab order
	::SetWindowPos(m_ColorSpectrum.m_hWnd, ::GetDlgItem(m_hWnd, IDC_FRAME), 
		0,0,0,0, SWP_NOMOVE|SWP_NOSIZE);

	m_ColorSpectrum.SetTooltipFormat(m_eFormat);

	SetValues();

	::SetFocus(m_ColorSpectrum.m_hWnd);

	m_bInitDone = TRUE;

	return FALSE;	// return TRUE unless you set the focus to a control
					// EXCEPTION: OCX Property Pages should return FALSE
}

//=============================================================================
void CTabCustom::SetValues()
//=============================================================================
{
	m_crNew = m_ColorSpectrum.GetRGB();
	m_nRed = GetRValue(m_crNew);
	m_nGreen = GetGValue(m_crNew);
	m_nBlue = GetBValue(m_crNew);

	BYTE h, s, l;
	m_ColorSpectrum.GetHSL(&h, &s, &l);
	m_nHue = h;
	m_nSat = s;
	m_nLum = l;

	UpdateData(FALSE);

	if (IsWindow(m_pParent->m_hWnd))
		m_pParent->SendMessage(WM_XCOLORPICKER_SELCHANGE, (WPARAM)m_crNew, 
			(LPARAM)g_nCtrlId);

	UpdateWindow();

	if (m_brushNew.GetSafeHandle())
		m_brushNew.DeleteObject();
	m_brushNew.CreateSolidBrush(m_crNew);
	GetDlgItem(IDC_NEW_COLOR)->Invalidate();
}

//=============================================================================
HBRUSH CTabCustom::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
//=============================================================================
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	switch ( pWnd->GetDlgCtrlID() )
	{
		case CTLCOLOR_DLG:
			hbr = (HBRUSH) ::GetStockObject(NULL_BRUSH);
			break;

		case CTLCOLOR_STATIC:
			pDC->SetBkMode(TRANSPARENT);
			hbr = (HBRUSH) ::GetStockObject(NULL_BRUSH);
			break;
		case IDC_NEW_COLOR:
			{
				if (m_brushNew.GetSafeHandle())
					hbr = m_brushNew;
			}break;

		case IDC_CURRENT_COLOR:
		{
			if (m_brushCurrent.GetSafeHandle())
				hbr = m_brushCurrent;
		}

		default:
			break;
	}

	return hbr;
}

//=============================================================================
BOOL CTabCustom::PreTranslateMessage(MSG* pMsg) 
//=============================================================================
{
	if (m_hAccel)
		::TranslateAccelerator(m_hWnd, m_hAccel, pMsg);
	
	return CDialog::PreTranslateMessage(pMsg);
}

//=============================================================================
// handler for WM_XCOLORPICKER_SELCHANGE
LRESULT CTabCustom::OnSelChange(WPARAM wParam, LPARAM lParam)
//=============================================================================
{
	TRACE(_T("CTabCustom::OnSelChange: color=0x%06X  id=%d\n"), wParam, lParam);

	SetValues();
	if (IsWindow(m_pParent->m_hWnd))
		m_pParent->SendMessage(WM_XCOLORPICKER_SELCHANGE, wParam, lParam);

	return 0;
}

//=============================================================================
// handler for WM_XCOLORPICKER_SELENDOK
//=============================================================================
LRESULT CTabCustom::OnSelendOk(WPARAM wParam, LPARAM lParam)
{
	TRACE(_T("CTabCustom::OnSelendOk:  color=0x%06X  id=%d\n"), wParam, lParam);

	SetValues();
	if (IsWindow(m_pParent->m_hWnd))
		m_pParent->SendMessage(WM_XCOLORPICKER_SELENDOK, wParam, lParam);
	return 0;
}

void CTabCustom::OnCancel()
{
	m_crNew = m_crCurrent;
	m_ColorSpectrum.SetRGB(m_crCurrent);
	SetValues();

	CDialog::OnCancel();
}

void CTabCustom::UpdateByRGB()
{
	// RGB
	m_crNew = RGB(m_nRed, m_nGreen, m_nBlue);
	m_ColorSpectrum.SetRGB(m_crNew);
	BYTE h, s, l;
	m_ColorSpectrum.GetHSL(&h, &s, &l);
	m_nHue = h;
	m_nSat = s;
	m_nLum = l;

	SetValues();
}

void CTabCustom::UpdateByHSL()
{
	// HSL
	m_ColorSpectrum.SetHSL((BYTE)m_nHue, (BYTE)m_nSat, (BYTE)m_nLum);
	m_crNew = m_ColorSpectrum.GetRGB();
	m_nRed = GetRValue(m_crNew);
	m_nGreen = GetGValue(m_crNew);
	m_nBlue = GetBValue(m_crNew);

	SetValues();
}

//=============================================================================
void CTabCustom::OnChangeEditRGB() 
//=============================================================================
{
	TRACE(_T("in CTabCustom::OnChangeEdit\n"));
	if (m_bInitDone)
	{
		UpdateData(TRUE);
		UpdateByRGB();
	}
}

//=============================================================================
void CTabCustom::OnChangeEditHSL() 
//=============================================================================
{
	TRACE(_T("in CTabCustom::OnChangeEdit\n"));
	if (m_bInitDone)
	{
		UpdateData(TRUE);
		UpdateByRGB();
	}
}

// on slider changed
void CTabCustom::OnHScroll(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar)
{
	if( pScrollBar->m_hWnd == m_sliderRed.m_hWnd ||
		 pScrollBar->m_hWnd == m_sliderGreen.m_hWnd ||
		  pScrollBar->m_hWnd == m_sliderBlue.m_hWnd)
	{
		m_nRed = m_sliderRed.GetPos();
		m_nGreen = m_sliderGreen.GetPos();
		m_nBlue = m_sliderBlue.GetPos();
		UpdateByRGB();
	}
	else
	{
		m_nHue = m_sliderHue.GetPos();
		m_nSat = m_sliderSat.GetPos();
		m_nLum = m_sliderLum.GetPos();
		UpdateByHSL();
	}
}

//=============================================================================
void CTabCustom::SetHSL(BYTE h, BYTE s, BYTE l)
//=============================================================================
{
	m_ColorSpectrum.SetHSL(h, s, l);
	SetValues();
}

//=============================================================================
void CTabCustom::SetRGB(COLORREF cr)
//=============================================================================
{
	m_ColorSpectrum.SetRGB(cr);
	SetValues();

	m_crCurrent = cr;

	if (m_brushCurrent.GetSafeHandle())
		m_brushCurrent.DeleteObject();
	m_brushCurrent.CreateSolidBrush(m_crCurrent);
}