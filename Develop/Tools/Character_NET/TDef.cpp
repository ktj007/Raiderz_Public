#include "stdafx.h"
#include "TDef.h"

string GetPureFileName(const string& szFileName)
{
	char drive[_MAX_DRIVE],dir[_MAX_DIR],fname[_MAX_FNAME],ext[_MAX_EXT];
	_splitpath(szFileName.c_str(),drive,dir,fname,ext);
	return string(fname);
}

string GetPathName(const string& szFileName)
{
	char drive[_MAX_DRIVE],dir[_MAX_DIR],fname[_MAX_FNAME],ext[_MAX_EXT];
	_splitpath(szFileName.c_str(),drive,dir,fname,ext);
	return string(dir);
}

string GetExtName(const string& szFileName)
{
	char drive[_MAX_DRIVE],dir[_MAX_DIR],fname[_MAX_FNAME],ext[_MAX_EXT];
	_splitpath(szFileName.c_str(),drive,dir,fname,ext);
	return string(ext);
}

string GetFolderName(const string& szFileName)
{
	char drive[_MAX_DRIVE],dir[_MAX_DIR],fname[_MAX_FNAME],ext[_MAX_EXT];
	_splitpath(szFileName.c_str(),drive,dir,fname,ext);

	string dirpath(dir);
	string dirpath2 = dirpath.substr(0, dirpath.size()-1);

	return GetPureFileName(dirpath2);
}

string GetRemovePathName(const string& strPathName)
{
	int nPos = strPathName.find_last_of("/\\");
	if(nPos != string::npos)
		return strPathName.substr(nPos+1);
	return strPathName;
}

string GetRemoveExtName(const string& strPathName)
{
	int nPos = strPathName.length();
	return strPathName.substr(0, nPos-4);
}

/************************************************************************/
/* blank앞부분을 넘겨줌                                                 */
/************************************************************************/
string GetRemoveBlanklast(const string& strPathName)
{
	int nPos = strPathName.find_first_of(" ");
	if(nPos != string::npos)
		return strPathName.substr(0, nPos);
	return strPathName;
}

string GetRemoveUnderBarlast(const string& strPathName)
{
	int nPos = strPathName.find_first_of("_");
	if(nPos != string::npos)
		return strPathName.substr(0, nPos);
	return "";
}

string GetRemoveUnderBarfirst(const string& strPathName)
{
	int nPos = strPathName.find_first_of("_");
	if(nPos != string::npos)
		return strPathName.substr(nPos+1);
	return "";
}

void ReplaceSlashToBackSlash(string& strPath)
{
	for(size_t i=0;i<strPath.length();i++)
		if(strPath[i]=='/')
			strPath[i]='\\';
}

void ReplaceUnderBarToBlank(string& strPath)
{
	for(size_t i=0;i<strPath.length();i++)
		if(strPath[i]=='_')
			strPath[i]=' ';
}

string GetFormatString(string& strPathName, const char *pFormat, ...)
{
	char temp[2048];

	va_list args;
	va_start(args,pFormat);
	vsprintf(temp,pFormat,args);
	va_end(args);

	strPathName = temp;

	return strPathName;
}

bool ExistFile(const char* str)
{
	HANDLE hDir;
	WIN32_FIND_DATA DirData = {0};

	hDir = FindFirstFile(str, &DirData);
	if( hDir == INVALID_HANDLE_VALUE)
		return false;

	return true;
}

bool IsEqualString( System::String^ Name, System::String^ FindName )
{
	array<String^>^split = Name->Split( );
	IEnumerator^ myEnum = split->GetEnumerator();
	while ( myEnum->MoveNext() )
	{
		String^ s = safe_cast<String^>(myEnum->Current);
		if(s->Contains(FindName) || s->StartsWith(FindName) || s->EndsWith(FindName))
		{
			return true;
		}
	}

	return false;
}
