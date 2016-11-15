#include "stdafx.h"

#include <conio.h>
#include <crtdbg.h>
#include <windows.h>

#include "cclistgen.h"

static void Replace( std::string& text, const std::string& find_token, const std::string& replace_token )
{
	size_t i = 0;
	while ((i = text.find(find_token)) != string::npos)
		text.replace(i, find_token.size(), replace_token);
}

static const bool isBlank(const std::string::value_type c)
{
	return (c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '(' || c == ')' || c == ';');
}

static const std::string trimBlanks(const std::string& str)
{
	std::string::size_type begin = 0;
	std::string::size_type end = str.length();

	while (begin != end && isBlank(str[begin])) ++begin;
	while (end != begin && isBlank(str[end - 1])) --end;

	return (std::string(str.begin() + begin, str.begin() + end));
}

bool CCListGen::Check_CCCheckFunc( char* szLine, string& outCCName )
{
	char* pdest;

	pdest = strstr(szLine, "CC_Check");

	char* n = pdest;

	if ( pdest != NULL )
	{
		int t = 0;
		while (*n != '\0' && *n != ')' && *n != ';')
		{
			t++;
			n++;
		}

		char szName[256] = "";
		if (t > 0)
		{
			strncpy(szName, pdest, t);
		}

		string strName = szName;

		Replace(strName, "CC_Check", "");
		Replace(strName, "(", "");
		Replace(strName, ")", "");
		strName = trimBlanks(strName);

		outCCName = strName;
		return true;
	}

	return false;
}

bool CCListGen::CheckAndGen(char *name)
{
	int nLength=(int)strlen(name);
	if(nLength<4) return false;

	char *extpos=name+nLength-4;
	char *extpos2=name+nLength-2;

	if(_stricmp(extpos,".cpp")==0 || _stricmp(extpos2,".h")==0)
	{
		CheckFile(name);
	}

	return false;
}

void CCListGen::GenCCList(const char *szPath)
{
	m_nRecursionLevel++;
	WIN32_FIND_DATA FileData; 
	HANDLE hSearch; 

	BOOL fFinished = FALSE; 

	char filetofind[MAX_PATH];
	sprintf_s(filetofind,"%s/*.*",szPath);

	hSearch = FindFirstFile(filetofind, &FileData); 
	if (hSearch == INVALID_HANDLE_VALUE) 
	{ 
		m_nRecursionLevel--;
		return;
	}

	while (!fFinished) 
	{ 
		char childpath[MAX_PATH];
		sprintf_s(childpath,"%s/%s",szPath,FileData.cFileName);

		//if(FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY &&
		//	strcmp(".",FileData.cFileName)!=0 &&
		//	strcmp("..",FileData.cFileName)!=0)
		//{
		//	GenCCList(childpath);
		//}

		if(CheckAndGen(childpath))
		{
			for(int i=0;i<m_nRecursionLevel;i++)
				printf("   ");
			printf("%s\n",FileData.cFileName);
		}

		if (!FindNextFile(hSearch, &FileData)) 
		{
			if (GetLastError() == ERROR_NO_MORE_FILES) 
			{ 
				fFinished = TRUE; 
			} 
			else 
			{ 
				printf("Couldn't find next file."); 
				m_nRecursionLevel--;
				return;
			} 
		}
	} 

	m_nRecursionLevel--;
}

CCListGen::CCListGen() : m_nRecursionLevel(0)
{

}

CCListGen::~CCListGen()
{
}

void CCListGen::CheckFile( char * name )
{
	FILE* fp;
	char line[256];
	if (fopen_s(&fp, name, "r") == 0)
	{
		while (fgets(line, 256, fp) != NULL)
		{
			string strName;
			if (Check_CCCheckFunc(line, strName))
			{
				setNames.insert(strName);
			}
		}
		fclose(fp);
	}
}

void CCListGen::Save( const char* szTargetFileName )
{
	FILE* pFile = fopen(szTargetFileName, "w");

	if (pFile)
	{
		for (set<string>::iterator itor = setNames.begin(); itor != setNames.end(); ++itor)
		{
			string strName = (*itor);

			char szText[256];
			sprintf_s(szText, "CC_Add(%s)\n", strName.c_str());
			fputs(szText, pFile);
		}
		fclose(pFile);
		pFile = 0;
	}

}