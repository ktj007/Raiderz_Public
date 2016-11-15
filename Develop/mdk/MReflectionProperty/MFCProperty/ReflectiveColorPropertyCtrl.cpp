#include "StdAfx.h"
#include "ReflectiveColorPropertyCtrl.h"
#include "XColorDialog/TabCustom.h"

/////////////////////////////////////////////////////////////////////////////
// CReflectiveColorPropertyCtrl object

IMPLEMENT_DYNAMIC(CReflectiveColorPropertyCtrl, CMFCPropertyGridColorProperty)

CReflectiveColorPropertyCtrl::CReflectiveColorPropertyCtrl(const CString& strName, const COLORREF& color, CPalette* pPalette, LPCTSTR lpszDescr, DWORD_PTR dwData) 
: CMFCPropertyGridColorProperty(strName, color, pPalette, lpszDescr, dwData)
, m_pXColorDialog(NULL)
{
}

CReflectiveColorPropertyCtrl::~CReflectiveColorPropertyCtrl()
{
	delete m_pXColorDialog;
}

void CReflectiveColorPropertyCtrl::OnClickButton(CPoint /*point*/)
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pWndList);

	m_bButtonIsDown = TRUE;
	Redraw();

	if( m_pXColorDialog == NULL )
	{
		m_pXColorDialog = new CTabCustom( m_Color, 0, 0, CXColorSpectrumCtrl::XCOLOR_TOOLTIP_NONE, m_pWndList, 0 );
		m_pXColorDialog->Create( _T("IDD_TAB_CUSTOM"), m_pWndList );
	}

	if( m_pXColorDialog->IsWindowVisible() )
	{
		m_pXColorDialog->ShowWindow( SW_HIDE );
	}else
	{
		m_pXColorDialog->SetRGB( m_Color );

		CPoint pt(m_pWndList->GetListRect().left + m_pWndList->GetLeftColumnWidth()+ 1, m_rectButton.bottom + 1);
		m_pWndList->ClientToScreen(&pt);

		m_pXColorDialog->SetWindowPos(NULL , pt.x, pt.y, 0, 0,  SWP_NOSIZE | SWP_NOZORDER );
		m_pXColorDialog->ShowWindow(SW_SHOW);
	}
}

void CReflectiveColorPropertyCtrl::OnKillSelection(CMFCPropertyGridProperty* /*pNewSel*/)
{
	if(m_pXColorDialog)
		m_pXColorDialog->ShowWindow( SW_HIDE );
}
