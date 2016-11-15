#include "stdafx.h"
#include "XGameApp.h"
#include "resource.h"
#include "version.h"
#include "SVNRevision.h"
#include "MLocale.h"


XGameApp::XGameApp() : XBaseApplication()
{
	m_nIconResID = IDI_ICON1;
}


XGameApp::~XGameApp()
{
}

const wchar_t* XGameApp::GetProductVersion()
{
	static wstring strVersion;
	strVersion = MLocale::ConvAnsiToUTF16(PRODUCT_VERSION);

	return strVersion.c_str();
}

const wchar_t* XGameApp::GetSVNRevision()
{
	static wstring strRevision;
	strRevision = MLocale::ConvAnsiToUTF16(SVN_REVISION);

	return strRevision.c_str();
}

const wchar_t* XGameApp::GetSVNDate()
{
	static wstring strSvnDate;
	strSvnDate = MLocale::ConvAnsiToUTF16(SVN_DATE);

	return strSvnDate.c_str(); 
}
