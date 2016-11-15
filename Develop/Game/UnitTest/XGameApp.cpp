#include "stdafx.h"
#include "XGameApp.h"
#include "resource.h"
#include "version.h"
#include "SVNRevision.h"


XGameApp::XGameApp() : XBaseApplication()
{
	m_nIconResID = IDI_ICON1;
}


XGameApp::~XGameApp()
{
}

const wchar_t* XGameApp::GetProductVersion()
{
	USES_CONVERSION_EX;

	static wstring strVersion;
	strVersion = A2W_EX(PRODUCT_VERSION, 0);

	return strVersion.c_str();
}

const wchar_t* XGameApp::GetSVNRevision()
{
	USES_CONVERSION_EX;

	static wstring strRevision;
	strRevision = A2W_EX(SVN_REVISION, 0);
	
	return strRevision.c_str();
}

const wchar_t* XGameApp::GetSVNDate()
{
	USES_CONVERSION_EX;

	static wstring strSvnDate;
	strSvnDate = A2W_EX(SVN_DATE, 0);
	
	return strSvnDate.c_str(); 
}
