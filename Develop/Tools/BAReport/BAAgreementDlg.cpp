// BAAgreementDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BAReport.h"
#include "BAAgreementDlg.h"
#include "BAReportDlg.h"
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// BAAgreementDlg dialog


BAAgreementDlg::BAAgreementDlg(CWnd* pParent /*=NULL*/)
	: CDialog(BAAgreementDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(BAAgreementDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void BAAgreementDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_RICHEDIT_DETAIL, m_Detail);
	//{{AFX_DATA_MAP(BAAgreementDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(BAAgreementDlg, CDialog)
	//{{AFX_MSG_MAP(BAAgreementDlg)
	ON_BN_CLICKED(ID_AGREE, OnAgree)
	ON_BN_CLICKED(ID_DISAGREE, OnDisagree)
	//}}AFX_MSG_MAP
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// BAAgreementDlg message handlers

BOOL BAAgreementDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_AGREEMENT);
	if (!g_bAgree)
		pEdit->SetWindowText(L"프로그램이 치명적인 오류를 발생하였으므로 관련 정보를 개발사에 전송하도록 권장합니다.\r\n\r\n본 프로그램은 프로그램 오류의 완전한 분석을 위하여 프로그램 오류정보 및 귀하의 시스템사양(운영체제/메모리/비디오카드 정보)를 본사에 전송할것입니다.\r\n\r\n전송된 자료는 프로그램 성능 향상을 위해서만 사용됩니다.\r\n정보를 보내시겠습니까?");

	//m_Detail.Create()
	m_Detail.SetWindowText(L"");

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void BAAgreementDlg::OnCancel()
{
}

void BAAgreementDlg::OnAgree() 
{
	CString strDetail;
	m_Detail.GetWindowText(strDetail);

	std::wstring strDetailString = (LPCTSTR)strDetail;

	if (strDetailString.length() > 1)
	{
		std::wstring strFileName = g_pFileTransfer->GetTextFileNameInQueue();
		if (strFileName.length() > 1)
		{
			FILE* fp;
			_wfopen_s(&fp, strFileName.c_str(), L"a");

			if (fp)
			{
				fprintf(fp, "\n--------------------------------------\n");
				fprintf(fp, "%s", ConvUTF16ToUTF8(L"크래쉬 정보\n").c_str());
				fprintf(fp, "--------------------------------------\n");
				fprintf(fp, "%s", ConvUTF16ToUTF8(strDetailString.c_str()).c_str());
				fprintf(fp, "\n");
				fclose(fp);
			}
		}
	}

	CBAReportDlg* pDlg = (CBAReportDlg*)AfxGetMainWnd();
	pDlg->Upload();
	DestroyWindow();
}

void BAAgreementDlg::OnDisagree() 
{
	CBAReportDlg* pDlg = (CBAReportDlg*)AfxGetMainWnd();
	pDlg->DestroyWindow();
}

BOOL BAAgreementDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if (LOWORD(wParam) == ID_AGREE)
		TRACE("SHIT!");
	return CDialog::OnCommand(wParam, lParam);
}

void BAAgreementDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

}

std::string	BAAgreementDlg::ConvUTF16ToUTF8(const wchar_t* pszString)
{
	// 필요한 길이의 버퍼를 준비한다.
	int nReqLen = WideCharToMultiByte(CP_UTF8, 0, pszString, wcslen(pszString) + 1, 0, 0, 0, 0);

	std::auto_ptr<char> szDest(new char[nReqLen]);


	// 버퍼에 유니코드로 변환된 문자열을 넣는다.
	int nLen = WideCharToMultiByte(CP_UTF8, 0, pszString, wcslen(pszString) + 1, szDest.get(), nReqLen, 0, 0);


	// 실패하면 빈문자열, 성공하면 변환된 문자열을 반환한다.
	if (nLen <= 0)
	{
		return std::string();
	}
	else
	{
		return szDest.get();
	}	
}
