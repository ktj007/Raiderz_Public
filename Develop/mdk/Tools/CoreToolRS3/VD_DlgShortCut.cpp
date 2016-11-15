// V_DlgShortCut.cpp : 구현 파일입니다.
//
#include "stdafx.h"
//#include "CoreToolRS3.h"
#include "VD_DlgShortCut.h"
#include <string>

// CDlgShortCut 대화 상자입니다.

//IMPLEMENT_DYNAMIC(CDlgShortCut, CDialog)
CDlgShortCut::CDlgShortCut()
	: CDialog(CDlgShortCut::IDD)
	, m_strShortcutText(_T(""))
{

}

CDlgShortCut::~CDlgShortCut()
{
}

BOOL CDlgShortCut::OnInitDialog(){
	CDialog::OnInitDialog();

	FILE *stream;
	char line[256];

	if( fopen_s( &stream, "hot_key.txt", "r" ) == 0 )
	{
	  while ( fgets( line, 256, stream ) != NULL)
	  {
		 m_strShortcutText = m_strShortcutText + line + CString("\r\n");
	  }
	  fclose( stream );
	}

	UpdateData(FALSE);

	return TRUE;
}

void CDlgShortCut::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strShortcutText);
}

void CDlgShortCut::OnOK()
{
	CDialog::OnOK();
}

BEGIN_MESSAGE_MAP(CDlgShortCut, CDialog)
END_MESSAGE_MAP()


