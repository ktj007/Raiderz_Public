#pragma once

#include "XBaseApplication.h"

struct XArgumentsParserResult
{
	_EXECUTE_MODE	nExecuteMode;
	XGameFrameID	nStartGameFrameID;
	wstring			strGameFrameArgs;
	XArgumentsParserResult() : nExecuteMode(EM_GAME), nStartGameFrameID(NONE_FRAME_ID) {}
};

class XArgumentsParser
{
protected:
	wstring ParseFileNameFromArguments(const wchar_t* szArgs);
	bool CheckPrefix(const wchar_t* szArgs, const wchar_t* szStr);
	bool ParseReplayFile( const wchar_t* szArgs, XArgumentsParserResult& ret );
	bool ParseLogin( const wchar_t* szArgs, XArgumentsParserResult& ret );
	bool ParseFrameName( const wchar_t* szArgs, XGameFrameManager* pGameFrameManager, XArgumentsParserResult& ret );

public: // Unittest위해 public으로 뺍니다.
	bool ParseLoginPmang( const wchar_t* szArgs, XArgumentsParserResult& ret );

public:
	XArgumentsParserResult Parse(const wchar_t* szArgs, const cml2::MCommandLine& commandLine, XGameFrameManager* pGameFrameManager=NULL);
};