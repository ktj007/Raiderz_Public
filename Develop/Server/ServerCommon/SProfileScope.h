#pragma once

#include "MStatisticsProfiler.h"
#include "MLocale.h"

class SProfileScope
{
private:
	std::wstring m_strFileName;
public:
	SProfileScope(const wchar_t* szFileName) 
	{
		m_strFileName = szFileName;
	}
	virtual ~SProfileScope() 
	{
		MSaveStatisticsProfile(MLocale::ConvUTF16ToAnsi(m_strFileName.c_str()).c_str());
	}
};
