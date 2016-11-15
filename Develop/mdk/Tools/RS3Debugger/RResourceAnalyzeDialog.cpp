// PerformanceViewerDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RS3Analyzer.h"
#include "RResourceAnalyzeDialog.h"
#include "RDebuggingInfo.h"

// CPerformanceViewerDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(RResourceAnalyzeDialog, CDialog)

RResourceAnalyzeDialog::RResourceAnalyzeDialog(CWnd* pParent /*=NULL*/)
	: CDialog(RResourceAnalyzeDialog::IDD, pParent)
{
	m_bUpdated =true;
	m_bSortLess =true;
}

RResourceAnalyzeDialog::~RResourceAnalyzeDialog()
{
}

void RResourceAnalyzeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MATERIAL_LIST, m_PerformanceTreeListCtrl);
	DDX_Control(pDX, IDC_EXPORT, m_btnExport);
	DDX_Control(pDX, IDC_UPDATE, m_btnUpdate);
}

void RResourceAnalyzeDialog::SetDefaultSize()
{
	static const int DX = 10;
	static const int DY = 10;
	static const int BASE_Y = 20;

	CRect PR;
	this->GetClientRect(&PR);

	if( true == m_PerformanceTreeListCtrl.IsInitialized() )
	{
		m_PerformanceTreeListCtrl.SetWindowPos(NULL, PR.left + DX ,	PR.top + DY + BASE_Y,
			(int)(PR.right - DX) - (PR.left + DX ) , (PR.bottom - DY) - (PR.top + DY) - 20, SWP_SHOWWINDOW);
	}
}

BOOL RResourceAnalyzeDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_PerformanceTreeListCtrl.Init();


	SetWindowPos(&wndNoTopMost,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);

	SetWindowText(DEBUGGER_RESOURCE_DIALOG_NAME); 
	
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void RResourceAnalyzeDialog::UpdateProfileInfo(char* pInputBuffer,int Size)
{
	MBuffer DataBuffer;
	DataBuffer.SetBuffer(pInputBuffer, Size);
	m_PerformanceTreeListCtrl.UpdateProfileInfo(DataBuffer);

	m_bUpdated =false;
}

void RResourceAnalyzeDialog::UpdateTreeItemData()
{
	if(m_bUpdated)
		return;

	m_PerformanceTreeListCtrl.UpdateItem();

	m_bUpdated =true;
}

BEGIN_MESSAGE_MAP(RResourceAnalyzeDialog, CDialog)
	ON_BN_CLICKED(IDC_UPDATE, &RResourceAnalyzeDialog::OnBnClickedUpdate)
	ON_BN_CLICKED(IDC_EXPORT, &RResourceAnalyzeDialog::OnBnClickedExport)
	ON_NOTIFY(HDN_ITEMCLICK, 0, &RResourceAnalyzeDialog::OnHdnItemclickMaterialList)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CPerformanceViewerDialog 메시지 처리기입니다.

void RResourceAnalyzeDialog::OnBnClickedUpdate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateTreeItemData();
}

void RResourceAnalyzeDialog::OnBnClickedExport()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog dlg(FALSE, "*.csv");
	
	dlg.m_ofn.lpstrFilter = "csv";
	dlg.m_ofn.Flags |= OFN_NOCHANGEDIR;

	if (dlg.DoModal() == IDOK)
	{
		CString fileName(dlg.GetPathName());

		FILE* fp;
		fopen_s(&fp, fileName, "w+" );

		if(fp)
		{
			m_PerformanceTreeListCtrl.ExportToFile(fp);

			fclose(fp);
		}
		else
		{
			MessageBox("파일을 여는데 실패했습니다.");
		}
	}
}

void RResourceAnalyzeDialog::OnHdnItemclickMaterialList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	m_PerformanceTreeListCtrl.Sort(phdr->iItem, m_bSortLess);
	m_bSortLess =!m_bSortLess;
}

void RResourceAnalyzeDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	SetDefaultSize();
}