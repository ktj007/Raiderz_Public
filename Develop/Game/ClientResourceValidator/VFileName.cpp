#include "stdafx.h"
#include "VFileName.h"

void VFileName::GetFileList( string strPathDir, vector<FILENAME_DATA>& vecOutFileList, vector<string>* vecAddFilteringString /*= NULL*/, vector<string>* vecSubFilteringString /*= NULL*/, FIND_ATTRIBUTE eFileAttribute /*= FA_FILE*/ )
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
						FILENAME_DATA stList;

						stList.strFileName		= DirData.cFileName;
						stList.strPureFileName	= DirData.cFileName;
						stList.strPath			= strPathDir;
						ToLower(stList);

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
					FILENAME_DATA stList;

					string strModelName		= DirData.cFileName;
					stList.strFileName		= strModelName;
					stList.strPureFileName	= strModelName.substr(0, strModelName.find("."));
					stList.strPath			= strPathDir;
					ToLower(stList);
					
					vecOutFileList.push_back(stList);
				}
			}
		}

		bRet = FindNextFile(hDir, &DirData);
	}

	FindClose(hDir);
}

bool VFileName::CheckSubFilteringString( const string& strName, vector<string>* vecSubFilteringString )
{
	if(vecSubFilteringString == NULL)
		return false;

	for(vector<string>::iterator itFilter = vecSubFilteringString->begin(); itFilter != vecSubFilteringString->end(); ++itFilter)
	{
		if(stristr(strName, *itFilter))
		{
			return true;
		}
	}

	return false;
}

bool VFileName::CheckAddFilteringString( const string& strName, vector<string>* vecAddFilteringString )
{
	if(vecAddFilteringString == NULL)
		return true;

	for(vector<string>::iterator itFilter = vecAddFilteringString->begin(); itFilter != vecAddFilteringString->end(); ++itFilter)
	{
		if(stristr(strName, *itFilter))
		{
			return true;
		}
	}

	return false;
}

bool VFileName::stristr( string str1, string str2 )
{
	_strlwr((char*)str1.c_str());
	_strlwr((char*)str2.c_str());
	if(str1.find(str2) != string::npos)
	{
		return true;
	}

	return false;
}

void VFileName::ToLower( FILENAME_DATA& fileData )
{
	// 소문자로...
	_strlwr( (char*)fileData.strPath.c_str());	
	_strlwr( (char*)fileData.strPureFileName.c_str());	
	_strlwr( (char*)fileData.strFileName.c_str());	
}