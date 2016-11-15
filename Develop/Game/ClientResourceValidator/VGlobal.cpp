#include "stdafx.h"
#include "VGlobal.h"
#include "VEffectInfoMgr.h"
#include "VSoundInfoMgr.h"
#include "VCameraInfoMgr.h"

bool ExistFile( const char* str )
{
	HANDLE hDir;
	WIN32_FIND_DATA DirData = {0};

	hDir = FindFirstFile(str, &DirData);
	if( hDir == INVALID_HANDLE_VALUE)
		return false;

	return true;
}

void CheckExistXmlFile( string strFileName, VErrorMsg* pErrorMsg )
{
	if(ExistFile(strFileName.c_str()))
	{
		pErrorMsg->AddErrorXml(strFileName);
	}
}

void CheckExistFile( string strFileName, VErrorMsg* pErrorMsg )
{
	if(ExistFile(strFileName.c_str()) == false)
	{
		pErrorMsg->AddErrorFile(strFileName);
	}
}

std::string TransToStringFromNumber( int nNum )
{
	char ch[32] = {0,};
	sprintf_s(ch, "%d", nNum);
	return ch;
}