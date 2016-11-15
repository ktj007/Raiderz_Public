// PPage.cpp : implementation file
//

#include "stdafx.h"
#include "EffectTool.h"
#include "PPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPPage property page

IMPLEMENT_DYNCREATE(CPPage, CPropertyPage)

CPPage::CPPage() : CPropertyPage(-1)	//  should not use!
{
	//{{AFX_DATA_INIT(CPPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPPage::CPPage(int nID) : CPropertyPage(nID)
{
	//{{AFX_DATA_INIT(CViewPropertyPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPPage::~CPPage()
{
}

void CPPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPPage, CPropertyPage)
	//{{AFX_MSG_MAP(CPPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPPage message handlers
