// DlgExportObj.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "VD_DlgExportObj.h"


// CDlgExportObj 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgExportObj, CDialog)

CDlgExportObj::CDlgExportObj(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgExportObj::IDD, pParent)
	, m_nLODLevel(0)
	, m_nWPart(0)
	, m_nHPart(0)
{

}

CDlgExportObj::~CDlgExportObj()
{
}

void CDlgExportObj::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_CBIndex(pDX, IDC_COMBO_NLEVEL, m_nLODLevel);
	DDX_CBIndex(pDX, IDC_COMBO_WPART, m_nWPart);
	DDX_CBIndex(pDX, IDC_COMBO_HPART, m_nHPart);
}


BEGIN_MESSAGE_MAP(CDlgExportObj, CDialog)
END_MESSAGE_MAP()

// CDlgExportObj 메시지 처리기입니다.
