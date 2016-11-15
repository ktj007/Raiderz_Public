///////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InfoWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInfoWnd
	
CInfoWnd::CInfoWnd(COLORREF cr)
{
    //static CBrush bkColor(RGB(255,129,129));
    static CBrush bkColor(cr);
    // Register the window class if it has not already been registered.
	WNDCLASS wndcls;
	HINSTANCE hInst = AfxGetInstanceHandle();
	if(!(::GetClassInfo(hInst, INFOWINDOW_CLASSNAME, &wndcls)))
	{
		// otherwise we need to register a new class
		wndcls.style			= CS_SAVEBITS;
		wndcls.lpfnWndProc		= ::DefWindowProc;
		wndcls.cbClsExtra		= wndcls.cbWndExtra = 0;
		wndcls.hInstance		= hInst;
		wndcls.hIcon			= NULL;
		wndcls.hCursor			= LoadCursor( hInst, IDC_ARROW );
        wndcls.hbrBackground	= (HBRUSH) bkColor;
		wndcls.lpszMenuName		= NULL;
		wndcls.lpszClassName	= INFOWINDOW_CLASSNAME;

		if (!AfxRegisterClass(&wndcls))
			AfxThrowResourceException();
	}
    m_nHorizontalOffset = 2;
    m_nVerticalOffset = 2;
    m_nAttach = 0;
}

CInfoWnd::~CInfoWnd()
{
 //   delete m_pBkColor;
}


BEGIN_MESSAGE_MAP(CInfoWnd, CWnd)
	//{{AFX_MSG_MAP(CInfoWnd)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CInfoWnd message handlers

BOOL CInfoWnd::Create(CWnd * pParentWnd)
{
	ASSERT_VALID(pParentWnd);

	DWORD dwStyle = WS_BORDER | WS_POPUP; 
	DWORD dwExStyle = WS_EX_TOOLWINDOW; //  | WS_EX_TOPMOST;
	m_pParentWnd = pParentWnd;

	return CreateEx(dwExStyle, INFOWINDOW_CLASSNAME, NULL, dwStyle, 
					CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
					pParentWnd->GetSafeHwnd(), NULL, NULL );
}


void CInfoWnd::Show(CWnd* p, LPCTSTR lpszTitleText)
{
    if (!IsWindow(GetSafeHwnd())) return;
	ASSERT( ::IsWindow( GetSafeHwnd() ) );
    ASSERT(p != NULL);
	ASSERT( ::IsWindow( p->GetSafeHwnd() ) );

    CRect rOriginalWnd;

	// If InfoWnd is already displayed, don't do anything.
	if( IsWindowVisible() ) 
		return;

    p->GetWindowRect(&rOriginalWnd); // Screen Coordinates   

    // Do not display the Infotip is app does not have focus
	if( GetFocus() == NULL )
		return;

    // determin the text extent !
	CClientDC dc(this);
	m_strTitle = _T(" ");
    m_strTitle += lpszTitleText;
    m_strTitle += _T(" ");

	CFont font, *pOldFont = NULL;
    pOldFont = dc.SelectObject( m_pParentWnd->GetFont() );

	CSize TextSize = dc.GetTextExtent( m_strTitle );

	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);
	TextSize.cx += tm.tmOverhang + 2;
	TextSize.cy += tm.tmDescent + 2;

	dc.SelectObject( pOldFont );


    if (m_nAttach == INFOWND_ATTACH_BOTTOM) {
        m_rectDisplay.left = rOriginalWnd.left;
        m_rectDisplay.top = rOriginalWnd.bottom + m_nVerticalOffset;
    }
    else if (m_nAttach & INFOWND_ATTACH_RIGHT) {
        m_rectDisplay.left = rOriginalWnd.right + m_nHorizontalOffset;
        m_rectDisplay.top = rOriginalWnd.top;
    }
    else if (m_nAttach & INFOWND_ATTACH_LEFT) {
        m_rectDisplay.left = rOriginalWnd.left - m_nHorizontalOffset - TextSize.cx;
        m_rectDisplay.top = rOriginalWnd.top;
    }
    else {
        m_rectDisplay.left = rOriginalWnd.left;
        m_rectDisplay.top = rOriginalWnd.top - TextSize.cy - m_nVerticalOffset;
    }
    m_rectDisplay.bottom = m_rectDisplay.top + TextSize.cy;
    m_rectDisplay.right = m_rectDisplay.left + TextSize.cx;


	SetWindowPos( &wndTop, 
        m_rectDisplay.left-3, 
        m_rectDisplay.top-1, 
		m_rectDisplay.Width()+3, 
        m_rectDisplay.Height(),
		SWP_SHOWWINDOW|SWP_NOACTIVATE );
        
}

void CInfoWnd::Hide()
{
  	if (!::IsWindow(GetSafeHwnd()))
		return;

	ShowWindow( SW_HIDE );
}

void CInfoWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

    TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);

	CFont *pFont = m_pParentWnd->GetFont(); 	// use same font as ctrl
	CFont *pFontDC = dc.SelectObject( pFont );
	int nHeight=0;

	CRect rect = m_rectDisplay;
	ScreenToClient(rect);

	dc.SetBkMode( TRANSPARENT ); 

	nHeight = dc.DrawText(m_strTitle, rect, 0 
		| DT_LEFT
		| DT_EDITCONTROL
		| DT_NOPREFIX
		| DT_WORDBREAK
		);


	dc.SelectObject( pFontDC );

	// Do not call CWnd::OnPaint() for painting messages
}

void CInfoWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CWnd::OnLButtonDown(nFlags, point);
}

void CInfoWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CWnd::OnLButtonUp(nFlags, point);
}
