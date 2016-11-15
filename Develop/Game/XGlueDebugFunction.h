#pragma once

#include "XGlueFunction.h"

/// lua에서 사용하는 디버그 API
class XGlueDebugFunction : public XGlueFunction
{
private:

public:
	virtual void DeclGlueFunc(MWLua* pLua);
	void AddLogFilter(const char* szFilter);
	void Console(const char* szCommand);

	void ConsoleCmd(const char* szCmd);			// 콘솔 명령어 입력
	void Log(const char* szMsg);


};
