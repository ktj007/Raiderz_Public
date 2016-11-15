#include "stdafx.h"
#include "GProcessManager.h"

////////////////////////////////////////////
GProcessManager::GProcessManager()
{

}

GProcessManager::~GProcessManager()
{
	int nSize = (int)m_vecProcesses.size();
	for (int i = 0; i < nSize; i++)
	{
		m_vecProcesses[i]->Terminate();
	}

	for (int i = 0; i < nSize; i++)
	{
		delete m_vecProcesses[i];
	}

	m_vecProcesses.clear();
}

void GProcessManager::Register(GProcess* pProcess)
{
	m_vecProcesses.push_back(pProcess);
}

void GProcessManager::Run()
{
	int nSize = (int)m_vecProcesses.size();
	for (int i = 0; i < nSize; i++)
	{
		m_vecProcesses[i]->Run();
	}
}
