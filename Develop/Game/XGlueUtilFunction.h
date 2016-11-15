#pragma once

#include "XGlueFunction.h"

/// lua에서 사용하는 유틸 API
class XGlueUtilFunction : public XGlueFunction
{
private:
	char		m_szBuffer[1024];
public:
	virtual void DeclGlueFunc(MWLua* pLua);
	int ParamCount(const char* szParams);
	const char* Param(const char* szParams, int nIndex);
};
