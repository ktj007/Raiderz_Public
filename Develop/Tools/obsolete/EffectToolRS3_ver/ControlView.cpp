// ControlView.cpp : implementation file
//

#include "stdafx.h"
#include "EffectTool.h"
#include "ControlView.h"
#include "RVType.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CControlView

extern CControlView*	g_ControlView;
extern CRView*			g_rView;

IMPLEMENT_DYNCREATE(CControlView, CFormView)

CControlView::CControlView()
	: CFormView(CControlView::IDD)
{
	//{{AFX_DATA_INIT(CControlView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	g_ControlView = this;
}

CControlView::~CControlView()
{
}

void CControlView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CControlView)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CControlView, CFormView)
	//{{AFX_MSG_MAP(CControlView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_WM_DROPFILES()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CControlView diagnostics

#ifdef _DEBUG
void CControlView::AssertValid() const
{
	CFormView::AssertValid();
}

void CControlView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CControlView message handlers



void CControlView::OnInitialUpdate() 
{
	// TODO: Add your specialized code here and/or call the base class
}

BOOL CControlView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
    if (!CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext))
		return FALSE;
   
    m_PSheet.AddPage(&m_Page1);
	m_PSheet.AddPage(&m_PageEmitter);
	m_PSheet.AddPage(&m_Page2);
	m_PSheet.AddPage(&m_Page3);
	//m_PSheet.AddPage(&m_Page4);

	// create a modeless property sheet
	if (!m_PSheet.Create(this)) {
		DestroyWindow();
		return FALSE;
	}

	// 미리생성? [8/11/2006 madduck]
	//m_PSheet.SetActivePage(&m_Page4);
	m_PSheet.SetActivePage(&m_Page3);
	m_PSheet.SetActivePage(&m_Page2);
	m_PSheet.SetActivePage(&m_PageEmitter);
	m_PSheet.SetActivePage(&m_Page1);

	return TRUE;
}

void CControlView::OnDropFiles(HDROP hDropInfo)
{
	char filename[256];
	int cnt = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
	if(cnt > 1)
		::MessageBox(NULL,"한번에 하나씩~","info",MB_OK);
	for(int i=0;i<1;i++) {
		DragQueryFile(hDropInfo,i,filename,sizeof(filename));
	}

	char name[MAX_PATH] = {0,};
	_getname(filename, name);

	// [9/26/2006 madduck]
	// TODO : 경로 설정 문제 ㅠ.ㅠ
	//REmitter::SetPathName(pathbuf);

	//g_rView->EffectFileOpen(filename);

	CFormView::OnDropFiles(hDropInfo);
}
