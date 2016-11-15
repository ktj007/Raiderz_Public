// V_OptionPage2.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "VD_OptionPage2.h"


// V_OptionPage2 대화 상자입니다.

IMPLEMENT_DYNAMIC(V_OptionPage2, CPropertyPage)

V_OptionPage2::V_OptionPage2()
	: CPropertyPage(V_OptionPage2::IDD)
	, m_strDataPath(_T(""))
	, m_strTilePath(_T(""))
	, m_strTreePath(_T(""))
	, m_strEluPath(_T(""))
	, m_strEftPath(_T(""))
	, m_strShaderPath(_T(""))
	, m_strTexturePath(_T(""))
{

}

V_OptionPage2::~V_OptionPage2()
{
}

void V_OptionPage2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ROOTPATH, m_strDataPath);
	DDX_Text(pDX, IDC_EDIT2, m_strTilePath);
	DDX_Text(pDX, IDC_EDIT7, m_strTreePath);
	DDX_Text(pDX, IDC_EDIT5, m_strEluPath);
	DDX_Text(pDX, IDC_EDIT6, m_strEftPath);
	DDX_Text(pDX, IDC_EDIT8, m_strShaderPath);
	DDX_Text(pDX, IDC_EDIT3, m_strTexturePath);

	DDX_Text(pDX, IDC_EDIT_SERVER_PATH, m_strServerPath);
}

BEGIN_MESSAGE_MAP(V_OptionPage2, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_ROOTPATH, &V_OptionPage2::OnBnClickedButtonRootpath)
	ON_BN_CLICKED(IDC_BUTTON_FIELD_PATH, &V_OptionPage2::OnBnClickedButtonFieldPath)
END_MESSAGE_MAP()


bool ShowFolderDialog(CString& outPathName, CString& strTitle)
{
	ITEMIDLIST *pidlBrowse;
	char pszPathname[1024];
	BROWSEINFO BrInfo;
	//BrInfo.hwndOwner = GetSafeHwnd(); 
	BrInfo.hwndOwner = 0;
	BrInfo.pidlRoot = NULL;
	memset(&BrInfo, 0, sizeof(BrInfo));
	BrInfo.pszDisplayName = pszPathname;
	BrInfo.lpszTitle = strTitle;
	BrInfo.ulFlags = BIF_RETURNONLYFSDIRS;
	pidlBrowse = ::SHBrowseForFolder(&BrInfo);

	if (NULL != pidlBrowse)
	{
	  ::SHGetPathFromIDList(pidlBrowse, pszPathname);
	  outPathName = pszPathname + CString("\\");
	  return true;
	}
	return false;
}

// V_OptionPage2 메시지 처리기입니다.

void V_OptionPage2::OnBnClickedButtonRootpath()
{
	CString path;
	if (ShowFolderDialog(path, CString("데이터 폴더 선택")))
	{
		m_strDataPath = path;
		UpdateData(FALSE);
	}
}

void V_OptionPage2::OnBnClickedButtonFieldPath()
{
	CString path;
	if (ShowFolderDialog(path, CString("스크립트 폴더 선택")))
	{
		m_strServerPath = path;
		UpdateData(FALSE);
	}
}
