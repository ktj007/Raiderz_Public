#pragma once

enum FIND_ATTRIBUTE
{
	FA_FILE,
	FA_DIRECTORY,
};

struct FILENAME_DATA
{
	string					strPath;
	string					strPureFileName;
	string					strFileName;
};

class VFileName
{
private:
	static bool				CheckSubFilteringString(const string& strName, vector<string>* vecSubFilteringString);
	static bool				CheckAddFilteringString(const string& strName, vector<string>* vecAddFilteringString);

	static void				ToLower(FILENAME_DATA& fileData);

public:
	static void				GetFileList(string strPathDir, vector<FILENAME_DATA>& vecOutFileList, vector<string>* vecAddFilteringString = NULL, vector<string>* vecSubFilteringString = NULL, FIND_ATTRIBUTE eFileAttribute = FA_FILE);
	static bool				stristr(string str1, string str2);
};
