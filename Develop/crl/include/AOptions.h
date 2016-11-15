#pragma once

#include "crlLib.h"
#include "AOptionSet.h"

/*
	커맨드라인 파서

	기본형식
		app.exe -log -file filename1 filename2 -path foldername

	사용법
		bool            isLog;
		vector<wstring> fileList;
		wstring         foldername;


		AOptions options;		
		
		options.Add(L"log",  L"로그 출력여부",     isLog);
		options.Add(L"file", L"복사대상 파일목록", fileList);
		options.Add(L"path", L"복사목표 폴더명",   foldername);


		options.Parser(commandLine);

		if (isLog)					{ 로그 처리 }
		if (0 < fileList.size())	{ 파일 처리 }
		if (!foldername.empty())	{ 폴더 처리 }
*/

class AOptionSetBase;

class CRL_API AOptions
{
public:
	AOptions();
	virtual ~AOptions();

	template<typename T>
	void Add(wstring option, wstring description, T& outValue)
	{
		m_listOptions.push_back(new AOptionSet<T>(option, description, outValue));
	}

	bool Parse(wstring commandLine);
	
	bool SetSwitch(wstring newSwitch);
	bool SetSeparator(wstring newSeparator);
	bool SetSplitter(wstring newSplitter);

private:
	bool ValidCommandLine(wstring commandLine);
	bool ParseOptions(wstring commandLine);
	
	wstring MakeEscape(wstring text);
	wstring MakeNotMatch(wstring text);
	wstring MakeMatchOption(void);

private:
	list<AOptionSetBase*> m_listOptions;
	wstring m_switch;
	wstring m_separator;
	wstring m_splitter;
};
