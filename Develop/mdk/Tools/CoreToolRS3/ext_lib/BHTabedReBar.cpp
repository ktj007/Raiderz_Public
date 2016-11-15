/********************************************************************
	Created:	5:6:2004   16:44
	Filename: 	BHTabedReBar
	File ext:	cpp (implementation file)
	Author:		Bahrudin Hrnjica
	e-mail:		bhrnjica@hotmail.com
	Web Page:	www.baha.dreamstation.com

	Disclaimer
	----------
	THIS SOFTWARE AND THE ACCOMPANYING FILES ARE DISTRIBUTED "AS IS" AND WITHOUT
	ANY WARRANTIES WHETHER EXPRESSED OR IMPLIED. NO REPONSIBILITIES FOR POSSIBLE
	DAMAGES OR EVEN FUNCTIONALITY CAN BE TAKEN. THE USER MUST ASSUME THE ENTIRE
	RISK OF USING THIS SOFTWARE.

	Terms of use
	------------
	This code may be used in compiled form in any way you desire. This
	file may be redistributed unmodified by any means PROVIDING it is 
	not sold for profit without the authors written consent, and 
	providing that this notice and the authors name and all copyright 
	notices remains intact. 

	An email letting me know how you are using it would be nice as well. 

	This file is provided "as is" with no expressed or implied warranty.
	The author accepts no liability for any damage/loss of business that
	this product may cause.

	History:
	Version: 1.0.0.109 First Release Version. 
	This control is based on the Paul Selormey article "Automatic Tab Bar for MDI programs" 
	posted on August 1, 1999 on www.codeguru.com. 
	BHTabedReBar is similar to standard MFC 
	CReBar which colect toolbars, dialogbars and statusbars and shows them as Tabed panes.


	TODO:  
	1)Imeplementation  Ownerdraw TabCtrl
	2)Docking to any side of Frame Window
	3)Implementation of chevrons
	4)

	*********************************************************************/
#include "stdafx.h"
#include "BHTabedReBar.h"
//#include "../resource.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CBHTabedReBar
IMPLEMENT_DYNAMIC(CBHTabedReBar, CControlBar)

//Constructor
CBHTabedReBar::CBHTabedReBar()
{
	m_nToobBarHeight=24;
	m_rectClient.SetRectEmpty();
	m_nActiveTab=0;
}
//destructor
CBHTabedReBar::~CBHTabedReBar()
{
	//Remove alocated bars
	POSITION pos = m_Bars.GetHeadPosition();
	while (pos != NULL)
	{
		TCB_ITEM* pBar = m_Bars.GetNext(pos);
		delete pBar;
	}
	m_Bars.RemoveAll();	
}
BEGIN_MESSAGE_MAP(CBHTabedReBar, CControlBar)
	//{{AFX_MSG_MAP(CBHTabedReBar)
	ON_WM_LBUTTONUP()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CREATE()
	ON_WM_WINDOWPOSCHANGED()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TABED_REBAR, OnTabSelChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// CBHTabedReBar message handlers
void CBHTabedReBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{	
	//UpdateDialogControls(m_pDockSite, bDisableIfNoHndler);
}

//Virtual member for TabedreBar Control creation
BOOL CBHTabedReBar::Create(CWnd* pParentWnd,
						   DWORD dwStyle /*= WS_CHILD | WS_VISIBLE | CBRS_BOTTOM*/,
						   UINT nID /*= AFX_IDW_STATUS_BAR*/)
{
	ASSERT_VALID(pParentWnd);   // must have a parent
	ASSERT (!((dwStyle & CBRS_SIZE_FIXED) && (dwStyle & CBRS_SIZE_DYNAMIC)));

	// save the style
	m_dwStyle = dwStyle;
  	m_dwStyle = dwStyle & CBRS_ALL; // save the control bar styles

	CString wndclass = AfxRegisterWndClass(CS_DBLCLKS, LoadCursor(NULL, IDC_ARROW),
		CreateSolidBrush(GetSysColor(COLOR_BTNFACE)), 0);

	dwStyle &= ~CBRS_ALL;
	dwStyle |= WS_CLIPCHILDREN | CCS_NOPARENTALIGN | CCS_NOMOVEY | CCS_NORESIZE;
	
	if (!CWnd::Create(wndclass, NULL, dwStyle, CRect(0,0,0,0),
		pParentWnd, nID))
		return FALSE;
	//This version support only TOP and BOTTOM docking 
	// TO DO: future version will be support other docking sides
	EnableDocking(CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM);
	return TRUE;
}

//After the control changed size, position, or Z-order, it is time to 
//reposion the tab control
void CBHTabedReBar::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CControlBar::OnWindowPosChanged(lpwndpos);

	CRect rect,RectT;
	rect.SetRect(0,0,lpwndpos->cx,lpwndpos->cy);
	rect.DeflateRect(3,3,3,3);	 

	// calcutaing the size and position of the tab control
	if(m_dwStyle & CBRS_FLOATING )   // If the ControlBar is Floating
	{
		m_tabctrl.MoveWindow(rect);
		rect.left+=2;
		rect.top+=22;
		rect.right-=4;
		rect.bottom-=4;
		m_rectClient=rect;
	}
	else if((m_dwStyle & CBRS_ALIGN_LEFT ))// If the ControlBar is Left aligned
	{	
		m_tabctrl.MoveWindow(5, 10, lpwndpos->cx - 7, lpwndpos->cy - 6);
		m_rectClient.SetRect(6,40,lpwndpos->cx- 7,lpwndpos->cy - 7);	 
	}
	else if((m_dwStyle & CBRS_ALIGN_TOP ))// If the ControlBar is Top aligned
	{
		m_tabctrl.MoveWindow(12, 3, lpwndpos->cx - 15, lpwndpos->cy - 6);
		m_rectClient.SetRect(14,25,lpwndpos->cx- 7,lpwndpos->cy - 7);
	}
	else if((m_dwStyle & CBRS_ALIGN_RIGHT ))// If the ControlBar is Right aligned
	{
		m_tabctrl.MoveWindow(2, 15, lpwndpos->cx - 15, lpwndpos->cy - 6);
		m_rectClient.SetRect(3,40,lpwndpos->cx- 7,lpwndpos->cy - 7);
	}
	else if((m_dwStyle & CBRS_ALIGN_BOTTOM ))// If the ControlBar is Bottom aligned
	{
		m_tabctrl.MoveWindow(12, 3, lpwndpos->cx - 15, lpwndpos->cy - 6);
		m_rectClient.SetRect(14,25,lpwndpos->cx- 7,lpwndpos->cy - 7);

	}
	//After repositionig the tab control we have to activate selected tab pane
	if (m_tabctrl.GetCurSel()!=-1)
	{
		TCB_ITEM *newMember=m_Bars.GetAt(m_Bars.FindIndex(m_tabctrl.GetCurSel()));
		newMember->pWnd->MoveWindow(m_rectClient);
		newMember->pWnd->EnableWindow(TRUE);
		newMember->pWnd->ShowWindow(SW_SHOW);
		m_nActiveTab = m_tabctrl.GetCurSel();
	}
}
// calculation of the horizontal and vertical size 
CSize CBHTabedReBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	CRect rc;
	m_pDockSite->GetClientRect(rc);
	int nHorzDockBarWidth = bStretch ? 32767 : rc.Width()+4;
	int nVertDockBarHeight = bStretch ? 32767 : rc.Height() + 4;

	if (bHorz)
		return CSize(nHorzDockBarWidth, m_nToobBarHeight);
	else
		return CSize(m_nToobBarHeight, nVertDockBarHeight);
}
//mouse message 
void CBHTabedReBar::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pDockBar != NULL)
	{
		// start the drag
		ASSERT(m_pDockContext != NULL);
		ClientToScreen(&point);
		m_pDockContext->StartDrag(point);
	}
	else
	{
		CWnd::OnLButtonDown(nFlags, point);
	}
}
//mouse message 
void CBHTabedReBar::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if (m_pDockBar != NULL)
	{
		// toggle docking
		ASSERT(m_pDockContext != NULL);
		m_pDockContext->ToggleDocking();
	}
	else
	{
		CWnd::OnLButtonDblClk(nFlags, point);
	}
}

int CBHTabedReBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	//Create the Tab Control
	CRect rect;
	m_tabctrl.Create(TCS_FORCEICONLEFT |TCS_HOTTRACK| TCS_RAGGEDRIGHT | WS_CHILD | WS_VISIBLE |WS_GROUP    ,
		rect, this, IDC_TABED_REBAR);

	// set "normal" GUI-font
	CFont *font = CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
	m_tabctrl.SetFont(font);
	return 0;
}
// This function Adds every bar 
BOOL CBHTabedReBar::AddBar(CControlBar* pBar,LPCTSTR pszText, bool _bRecalcLayout)
{
	TCB_ITEM *pMember=new TCB_ITEM;
	pMember->pWnd=pBar;
	pMember->strTitle=pszText;
	m_Bars.AddTail(pMember);

	int nBars = (int)m_Bars.GetCount();

	// Call this function to get real size of  
	// added control Bar

	CRect rect;
	pBar->GetWindowRect(rect);

	if(_bRecalcLayout)
	{
		((CFrameWnd*)GetParent())->RecalcLayout();
		pBar->GetWindowRect(rect);
		if(m_nToobBarHeight<(rect.Height()+32))
			m_nToobBarHeight=rect.Height()+32;
	}
	

	//Calculate the bigest height and assing to m_nToobBarHeight
	

	if (nBars!=1)
	{
		pBar->EnableWindow(FALSE);
		pBar->ShowWindow(SW_HIDE);
	}
	else
	{
		pBar->MoveWindow(m_rectClient);
		pBar->EnableWindow(TRUE);
		pBar->ShowWindow(SW_SHOW);
	}

	//Set new parent for added bar
	pBar->SetParent(this);


	TC_ITEM tci;
	tci.mask = TCIF_TEXT | TCIF_IMAGE;
	tci.pszText = (LPTSTR)(LPCTSTR)pszText;
	tci.iImage = nBars-1;
	m_tabctrl.InsertItem(nBars, &tci);

	return TRUE;

}
//Notifitacion message of tab selection
void CBHTabedReBar::OnTabSelChange(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (m_tabctrl.GetCurSel()!=-1 && m_tabctrl.GetCurSel()!=m_nActiveTab)
	{
		TCB_ITEM *newMember=m_Bars.GetAt(m_Bars.FindIndex(m_tabctrl.GetCurSel()));
		TCB_ITEM *oldMember=NULL;

		if (m_nActiveTab!=-1)
		{
			oldMember=m_Bars.GetAt(m_Bars.FindIndex(m_nActiveTab));
			oldMember->pWnd->EnableWindow(FALSE);
			oldMember->pWnd->ShowWindow(SW_HIDE);
		}
		newMember->pWnd->MoveWindow(m_rectClient);
		newMember->pWnd->EnableWindow(TRUE);
		newMember->pWnd->ShowWindow(SW_SHOW);
		
		m_nActiveTab = m_tabctrl.GetCurSel();

		//by pok, 펼쳐지면 펼쳐진것 번호하고 펼쳐진 DialogBar를 보냄
		AfxGetMainWnd()->SendMessage(WM_USER_REBAR_EXPANDED, (WPARAM)newMember->pWnd, (LPARAM)m_nActiveTab );
	}
}

//by pok, change tab Selection
void CBHTabedReBar::ChangeTabSelection(int _num){

	TCB_ITEM *newMember=m_Bars.GetAt(m_Bars.FindIndex(_num));
	TCB_ITEM *oldMember=NULL;

	if (m_nActiveTab!=-1)
	{
		oldMember=m_Bars.GetAt(m_Bars.FindIndex(m_nActiveTab));
		oldMember->pWnd->EnableWindow(FALSE);
		oldMember->pWnd->ShowWindow(SW_HIDE);
	}

	newMember->pWnd->MoveWindow(m_rectClient);
	newMember->pWnd->EnableWindow(TRUE);
	newMember->pWnd->ShowWindow(SW_SHOW);
	
	m_nActiveTab = _num;
	m_tabctrl.SetCurSel(_num);

	//by pok, 펼쳐지면 펼쳐진것 번호하고 메시지를 보냄
	//AfxGetMainWnd()->SendMessage(WM_USER_REBAR_EXPANDED, m_nActiveTab);
	//아니다. 이건 수동적으로 client 소스단에서 해주는 일이므로 그쪽에게 열림에 대한 전권을 위임
}

// virtual membera for painting
void CBHTabedReBar::DoPaint(CDC* pDC)
{
	if(!(m_dwStyle & CBRS_FLOATING) )
		DrawGripper(pDC);
	Default();
	return;
}
// Gripper drawing
void CBHTabedReBar::DrawGripper(CDC* pDC)
{
	if( (m_dwStyle & CBRS_FLOATING) || m_dwDockStyle == 0 )
		return;

	COLORREF clrBtnHilight = ::GetSysColor(COLOR_BTNHILIGHT);
	COLORREF clrBtnShadow  = ::GetSysColor(COLOR_BTNSHADOW);

	CRect rcGrip;					 
	GetWindowRect(&rcGrip);
	ScreenToClient(&rcGrip);
	rcGrip.OffsetRect(-rcGrip.left, -rcGrip.top);

	if(m_dwStyle & CBRS_ORIENT_HORZ) 
	{
		// gripper at left
		rcGrip.DeflateRect(3, 3);
		rcGrip.right = rcGrip.left+3;
		pDC->Draw3dRect(rcGrip, clrBtnHilight, clrBtnShadow);
		rcGrip.OffsetRect(3, 0);
		pDC->Draw3dRect(rcGrip,	clrBtnHilight, clrBtnShadow);
	}
	else 
	{
		// gripper at top
		rcGrip.DeflateRect(3, 3);
		rcGrip.bottom = rcGrip.top+3;
		pDC->Draw3dRect(rcGrip, clrBtnHilight, clrBtnShadow);
		rcGrip.OffsetRect(0, 3);
		pDC->Draw3dRect(rcGrip, clrBtnHilight, clrBtnShadow);
	}
}
// Access tab control
CTabCtrl& CBHTabedReBar::GetTabCtrl()  const
{	
	return *(CTabCtrl*)&m_tabctrl;
}
//
//void CBHTabedReBar::AddDisableTab(int _nPage)
//{
//	m_vecDisableTab_.push_back(_nPage);
//
//	//글자 바꾸기
//	CTabCtrl* pTab = static_cast<CTabCtrl*>(&m_tabctrl);
//	ASSERT (pTab);
//	TC_ITEM ti;
//	char szText[100];
//	ti.mask = TCIF_TEXT;
//	ti.pszText = szText;
//	ti.cchTextMax = 100;
//	VERIFY (pTab->GetItem (_nPage, &ti));
//	strcat (szText, " (x)");
//	VERIFY (pTab->SetItem (_nPage, &ti));
//}

void CBHTabedReBar::ClearBar()
{
	for(POSITION pos = m_Bars.GetHeadPosition(); pos != NULL; )
	{
		TCB_ITEM *pItem = m_Bars.GetNext(pos);
		SAFE_DELETE(pItem);
	}
	m_Bars.RemoveAll();
	m_tabctrl.DeleteAllItems();
	m_nActiveTab = -1;
}
//void CBHTabedReBar::SetEnalbeAll()
//{
//	m_vecDisableTab_.clear();
//
//	for(int i = 0; i < (int)m_Bars.GetCount(); i++)
//	{
//		// take out " - Disabled" from tab label
//		CTabCtrl* pTab = static_cast<CTabCtrl*>(&m_tabctrl);
//		ASSERT (pTab);
//		TC_ITEM ti;
//		char szText[100];
//		ti.mask = TCIF_TEXT;
//		ti.pszText = szText;
//		ti.cchTextMax = 100;
//		VERIFY (pTab->GetItem (i, &ti));
//		char * pFound = strstr (szText, " (x)");
//		if (pFound)
//		{
//			*pFound = '\0';
//			VERIFY (pTab->SetItem (i, &ti));
//		}
//	}
//}

//bool CBHTabedReBar::IsEnableTab_(int _nPage)
//{
//	if(m_vecDisableTab_.empty())
//		return true;
//
//	std::vector<int>::iterator itr;
//	for( itr = m_vecDisableTab_.begin(); itr != m_vecDisableTab_.end(); itr++ )
//	{
//		if(_nPage == *itr)
//			return false;
//	}
//	return true;
//}