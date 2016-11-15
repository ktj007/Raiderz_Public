#include "stdafx.h"
#include "SIOCPServiceAdapter.h"
#include "SServerApplication.h"
#include "MCommandLine.h"
#include "MLocale.h"

SIOCPServiceAdapter::SIOCPServiceAdapter(SServerApplication* pApplication, SServerAppType applicationType, wstring strServerName)
: SServiceAdapter(pApplication, applicationType, strServerName)
, m_isRunningApplication(false)
{
}

SIOCPServiceAdapter::~SIOCPServiceAdapter()
{
}

bool SIOCPServiceAdapter::StartServiceThread()
{
	return MIOCP::Init(1, 0, 300);
}

void SIOCPServiceAdapter::AfterGQCS()
{
	if (false == m_isRunningApplication)
	{
		m_isRunningApplication = true;
		RunApplication(this);
	}
}
