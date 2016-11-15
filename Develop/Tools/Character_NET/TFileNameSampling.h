#pragma once

enum FIND_ATTRIBUTE
{
	FA_FILE,
	FA_DIRECTORY,
};

struct FILE_LIST
{
	string					strPath;
	string					strPureFileName;
	string					strFileName;
};

class TFileNameSampling
{
private:
	static bool				CheckSubFilteringString(const string& strName, vector<string>* vecSubFilteringString);
	static bool				CheckAddFilteringString(const string& strName, vector<string>* vecAddFilteringString);

public:
	static void				GetFileList(string strPathDir, vector<FILE_LIST>& vecOutFileList, vector<string>* vecAddFilteringString = NULL, vector<string>* vecSubFilteringString = NULL, FIND_ATTRIBUTE eFileAttribute = FA_FILE);
	static bool				GetFileModifyTime( const char* pFileName, FILETIME& outTime );
	static bool				CheckXmlFileInfo(string strFileName);
};
