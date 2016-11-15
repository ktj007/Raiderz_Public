#pragma once

#include "SServiceAdapter.h"
#include "MIOCP.h"

class SIOCPServiceAdapter : public MIOCP, public SServiceAdapter
{
public:
	SIOCPServiceAdapter(SServerApplication* pApplication, SServerAppType m_applicationType, wstring strServerName);
	virtual ~SIOCPServiceAdapter();

private:
	bool StartServiceThread();
	void AfterGQCS();

private:
	bool m_isRunningApplication;
};
