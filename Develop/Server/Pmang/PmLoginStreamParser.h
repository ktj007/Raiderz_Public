#pragma once

#include "PmLib.h"
#include "PmDef.h"

class PMANG_API PmLoginStreamParser
{
public:
	PmLoginStreamParser(wstring strLoginStream);

	bool Parse();

	wstring GetLoginStream() const;	
	wstring GetHashKeyStream() const;
	wstring GetOptionStream() const;

	time_t GetTimeStamp() const;		
	wstring GetHashedValue() const;
	
	PmUserData GetUserData() const;

private:
	bool ParseAppendedData();

private:
	wstring m_strLoginStream;
	wstring m_strAppendedStream;
	wstring m_strHashKeyStream;

	time_t m_timeStamp;
	wstring m_strHashedValue;

	PmUserData m_pmangUserData;
};
