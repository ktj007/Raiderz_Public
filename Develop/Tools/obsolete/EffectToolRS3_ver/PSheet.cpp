// psheet.cpp : implementation file
//

#include "stdafx.h"
#include "EffectTool.h"
#include "PSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CControlView*  g_ControlView;

/////////////////////////////////////////////////////////////////////////////
// CPSheet

IMPLEMENT_DYNAMIC(CPSheet, CPropertySheet)

CPSheet::CPSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CPSheet::CPSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

CPSheet::CPSheet()
{
}

CPSheet::~CPSheet()
{
}


BEGIN_MESSAGE_MAP(CPSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CPSheet)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_ADJUST, OnAdjust)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPSheet message handlers

LRESULT CPSheet::OnAdjust(WPARAM wParam, LPARAM lParam)
{
    AdjustPages();
	return 0;
}

// Adjusts the size and position of Tabs and PropertyPages
//	
void CPSheet::AdjustPages()
{
   	CRect rect;		
	g_ControlView->GetClientRect(&rect);

	//rect.top += 50;
    MoveWindow(rect, TRUE);	
}

BOOL CPSheet::OnInitDialog() 
{
	//ModifyStyleEx(0, WS_EX_CONTROLPARENT);
	BOOL bResult = CPropertySheet::OnInitDialog();
	
	CFont font;
    VERIFY(font.CreateFont(
    12,                        // nHeight
    0,                         // nWidth
    0,                         // nEscapement
    0,                         // nOrientation
    FW_BOLD,                   // nWeight
    FALSE,                     // bItalic
    FALSE,                     // bUnderline
    0,                         // cStrikeOut
    ANSI_CHARSET,              // nCharSet
    OUT_DEFAULT_PRECIS,        // nOutPrecision
    CLIP_DEFAULT_PRECIS,       // nClipPrecision
    DEFAULT_QUALITY,           // nQuality
    DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
    "Arial")); 		

	CTabCtrl* pTab = GetTabControl();
	pTab->SetFont(&font, true);

	// Resize the CTabCtrl and PropertyPages
	PostMessage(WM_ADJUST);
	
	return bResult;
}

BOOL CPSheet::Create(CWnd *pParentWnd, DWORD dwStyle, DWORD dwExStyle)
{
	// Default to a Child Window overriding normal CPropertySheet Style
	if (dwStyle == -1)
		dwStyle = DS_3DLOOK | DS_CONTEXTHELP | DS_SETFONT |	WS_VISIBLE | WS_CHILD;
	//폼뷰에 Sheet를 붙이기 위한 설정
	
	dwExStyle = dwExStyle | WS_EX_CONTROLPARENT; // Handle tab key
	
	if (!CPropertySheet::Create(pParentWnd,dwStyle,dwExStyle))
		return FALSE;
	
	return TRUE;
}

void CPSheet::OnSize(UINT nType, int cx, int cy) 
{
	CPropertySheet::OnSize(nType, cx, cy);	

	PostMessage(WM_ADJUST); // adjust the pages	
}
