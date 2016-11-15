#include "stdafx.h"
#include "GRuntimeValidator.h"
#include "GRuntimeValidatingReporter.h"
#include "GRuntimeLuaLogger.h"
#include "GRuntimeDBLogger.h"
#include "GRuntimeTestLogger.h"

GRuntimeValidator::GRuntimeValidator()
{
	m_pReporter = new GRuntimeValidatingReporter;

	m_vecLoggers.push_back(new GRuntimeLuaLogger(*m_pReporter));
	m_vecLoggers.push_back(new GRuntimeDBLogger(*m_pReporter));
	m_vecLoggers.push_back(new GRuntimeTestLogger(*m_pReporter));
}

GRuntimeValidator::~GRuntimeValidator()
{
	for each(GRuntimeValidatingLogger* pLogger in m_vecLoggers)
	{
		delete pLogger;
	}

	delete m_pReporter;
}

void GRuntimeValidator::Start(void)
{
	for each(GRuntimeValidatingLogger* pLogger in m_vecLoggers)
	{
		pLogger->Start();
	}
}

void GRuntimeValidator::Stop(void)
{
	for each(GRuntimeValidatingLogger* pLogger in m_vecLoggers)
	{
		pLogger->Stop();
	}
}
