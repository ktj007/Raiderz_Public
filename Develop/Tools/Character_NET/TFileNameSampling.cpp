#include "stdafx.h"
#include "TFileNameSampling.h"

void TFileNameSampling::GetFileList( string strPathDir, vector<FILE_LIST>& vecOutFileList, vector<string>* vecAddFilteringString /*= NULL*/, vector<string>* vecSubFilteringString /*= NULL*/, FIND_ATTRIBUTE eFileAttribute /*= FA_FILE*/ )
{
	HANDLE hDir;
	WIN32_FIND_DATA DirData = {0};
	BOOL bRet = TRUE;

	string strReadFileList = strPathDir + "*.*";
	hDir = FindFirstFile(strReadFileList.c_str(), &DirData);
	while( (hDir != INVALID_HANDLE_VALUE) && bRet )
	{
		if((DirData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == false &&
			(DirData.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY) == false)
		{
			if (DirData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
			{
				if ((strcmp(DirData.cFileName, ".") != 0) &&
					(strcmp(DirData.cFileName, "..") != 0))
				{
					if(eFileAttribute == FA_DIRECTORY)
					{
						FILE_LIST stList;

						stList.strFileName		= DirData.cFileName;
						stList.strPureFileName	= DirData.cFileName;
						stList.strPath			= strPathDir;
						vecOutFileList.push_back(stList);	
					}
					else
						GetFileList(strPathDir + DirData.cFileName + "/", vecOutFileList, vecAddFilteringString, vecSubFilteringString);
				}
			}
			else if(eFileAttribute == FA_FILE)
			{
				if(CheckSubFilteringString(DirData.cFileName, vecSubFilteringString) == false &&
					CheckAddFilteringString(DirData.cFileName, vecAddFilteringString) == true) 
				{
					FILE_LIST stList;

					string strModelName		= DirData.cFileName;
					stList.strFileName		= strModelName;
					stList.strPureFileName	= strModelName.substr(0, strModelName.find("."));
					stList.strPath			= strPathDir;
					vecOutFileList.push_back(stList);
				}
			}
		}

		bRet = FindNextFile(hDir, &DirData);
	}

	FindClose(hDir);
}

bool TFileNameSampling::CheckSubFilteringString( const string& strName, vector<string>* vecSubFilteringString )
{
	if(vecSubFilteringString == NULL)
		return false;

	for(vector<string>::iterator itFilter = vecSubFilteringString->begin(); itFilter != vecSubFilteringString->end(); ++itFilter)
	{
		if(TStrings::stristr(strName, *itFilter))
		{
			return true;
		}
	}

	return false;
}

bool TFileNameSampling::CheckAddFilteringString( const string& strName, vector<string>* vecAddFilteringString )
{
	if(vecAddFilteringString == NULL)
		return true;

	for(vector<string>::iterator itFilter = vecAddFilteringString->begin(); itFilter != vecAddFilteringString->end(); ++itFilter)
	{
		if(TStrings::stristr(strName, *itFilter))
		{
			return true;
		}
	}

	return false;
}

bool TFileNameSampling::GetFileModifyTime( const char* pFileName, FILETIME& outTime )
{
	HANDLE hDir;
	WIN32_FIND_DATA DirData = {0};

	hDir = FindFirstFile(pFileName, &DirData);
	if(hDir != INVALID_HANDLE_VALUE)
	{
		outTime = DirData.ftLastWriteTime;
		return true;
	}

	return false;
}

bool TFileNameSampling::CheckXmlFileInfo( string strFileName )
{
	MXml xml;
	if (!xml.LoadFile(strFileName.c_str()))
	{
		string strErrorFileName = "¿¡·¯ : Failed To Load " + strFileName.substr(strFileName.rfind("/") + 1, strFileName.length()) + "\r";
		mlog(strErrorFileName.c_str());
		return false;
	}

	return true;
}