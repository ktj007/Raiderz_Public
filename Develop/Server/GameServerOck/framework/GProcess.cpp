#include "stdafx.h"
#include "GProcess.h"


GProcess::GProcess(MUID& uid) : m_UID(uid), m_nStat(PS_RUNNING)
{

}

GProcess::~GProcess()
{

}

void GProcess::Run()
{
	OnRun();
}

void GProcess::Terminate()
{
	OnTerminate();
}

const MUID GProcess::GetUID(void) const
{
	return m_UID;
}

