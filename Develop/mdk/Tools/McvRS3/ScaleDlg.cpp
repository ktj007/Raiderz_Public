#include "stdafx.h"

#include "ScaleDlg.h"

using namespace rs3;

IMPLEMENT_DYNAMIC(CScaleDlg, CDialog)
CScaleDlg::CScaleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CScaleDlg::IDD, pParent)
{
}

CScaleDlg::~CScaleDlg()
{
}

void CScaleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SX, m_sx);
	DDX_Control(pDX, IDC_SY, m_sy);
	DDX_Control(pDX, IDC_SZ, m_sz);
}

BEGIN_MESSAGE_MAP(CScaleDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_NOTIFY(NM_THEMECHANGED, IDC_SX, OnNMThemeChangedSx)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SX, OnNMReleasedcaptureSx)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SY, OnNMReleasedcaptureSy)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SZ, OnNMReleasedcaptureSz)
END_MESSAGE_MAP()


void CScaleDlg::OnBnClickedOk()
{
	CMcvMain* pMain = GetMain();

	if(!pMain) return;

	pMain->m_vModelScale.x = m_sx.GetPos()*0.1f;
	pMain->m_vModelScale.y = m_sy.GetPos()*0.1f;
	pMain->m_vModelScale.z = m_sz.GetPos()*0.1f;

	GetMainView()->ToggleScaleDlg();

	OnOK();
}

BOOL CScaleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_sx.SetRange(1,50);
	m_sx.SetPos(10);

	m_sy.SetRange(1,50);
	m_sy.SetPos(10);

	m_sz.SetRange(1,50);
	m_sz.SetPos(10);
	
	return TRUE;  
}

void CScaleDlg::OnNMThemeChangedSx(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
}

void CScaleDlg::OnNMReleasedcaptureSx(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	CMcvMain* pMain = GetMain();

	if(!pMain) return;

	pMain->m_vModelScale.x = m_sx.GetPos()*0.1f;
}

void CScaleDlg::OnNMReleasedcaptureSy(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	CMcvMain* pMain = GetMain();

	if(!pMain) return;

	pMain->m_vModelScale.y = m_sy.GetPos()*0.1f;
}

void CScaleDlg::OnNMReleasedcaptureSz(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	CMcvMain* pMain = GetMain();

	if(!pMain) return;

	pMain->m_vModelScale.z = m_sz.GetPos()*0.1f;
}
