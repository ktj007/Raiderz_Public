#include "stdafx.h"
#include "XDummyReporter.h"
#include "XBirdDummyAgent.h"

XDummyReporter::XDummyReporter( XBirdDummyAgentMgr* pAgentMgr ) : m_pAgentMgr(pAgentMgr), m_nLastUpdateTime(0)
{

}

void XDummyReporter::UpdateTotalPacketBytes()
{
	uint32 nTotalSendTraffic = 0;
	uint32 nTotalRecvTraffic = 0;
	uint32 nTotalSendBytes = 0, nTotalRecvBytes = 0;

	for (size_t i = 0; i < m_pAgentMgr->GetSize(); i++)
	{
		XBirdDummyAgent* pAgent = m_pAgentMgr->GetAgent(i);
		int nSendTraffic = 0;
		int nRecvTraffic = 0;
		pAgent->GetClient()->GetTraffic(nSendTraffic, nRecvTraffic);

		nTotalSendTraffic += (uint32)nSendTraffic;
		nTotalRecvTraffic += (uint32)nRecvTraffic;

		nTotalSendBytes += pAgent->GetClient()->GetTotalSendBytes();
		nTotalRecvBytes += pAgent->GetClient()->GetTotalRecvBytes();
	}
	m_TotalInfo.m_nSecRecvBytes = nTotalRecvTraffic;
	m_TotalInfo.m_nSecSendBytes = nTotalSendTraffic;
	m_TotalInfo.m_nTotalSendBytes = nTotalSendBytes;
	m_TotalInfo.m_nTotalRecvBytes = nTotalRecvBytes;
}

void XDummyReporter::Update()
{
	if (m_nLastUpdateTime == 0) m_nLastUpdateTime = timeGetTime();
	if (timeGetTime() - m_nLastUpdateTime < 1000) return;
	m_nLastUpdateTime = timeGetTime();

	UpdateTotalInfo();
	UpdateAgentInfo();
}

XDummyReporter::~XDummyReporter()
{

}

void XDummyReporter::UpdateTotalInfo()
{
	if (m_TotalInfo.m_nLastCheckTime == 0) m_TotalInfo.m_nLastCheckTime = timeGetTime();

	uint32 elapsedTime = timeGetTime() - m_TotalInfo.m_nLastCheckTime;
	m_TotalInfo.m_nTotalRunningTime += elapsedTime;
	m_TotalInfo.m_nLastCheckTime = timeGetTime();

	UpdateTotalPacketBytes();
}

void XDummyReporter::UpdateAgentInfo()
{
	m_vecAgentInfo.clear();

	XDummyAgentInfo agentInfo;
	int index = 0;

	m_vecAgentInfo.push_back(agentInfo);

	for (size_t i = 0; i < m_pAgentMgr->GetSize(); i++)
	{
		XBirdDummyAgent* pAgent = m_pAgentMgr->GetAgent(i);
		wstring strAgentName = pAgent->GetName();

		if (i !=0 && m_vecAgentInfo[index].strAgentName != strAgentName)
		{
			agentInfo.Clear();
			m_vecAgentInfo.push_back(agentInfo);
			index++;
		}

		if (m_vecAgentInfo[index].strAgentName.empty()) m_vecAgentInfo[index].strAgentName = strAgentName;
		m_vecAgentInfo[index].m_nAgentCount++;
		if (XBirdDummyAgent::DAS_RUNNING == pAgent->GetStatus())
			m_vecAgentInfo[index].m_nRunningAgentCount++;
		m_vecAgentInfo[index].m_nTotalRunningCount += pAgent->GetConnCount();
	}

}