#include "StdAfx.h"
#include "GNPCMgr.h"
#include "GEntityNPC.h"

GNPCMgr g_NPCMgr;

GNPCMgr::GNPCMgr(void)
: m_nLastUpdatedIndex(0)
, fTickElapsedTime(0.0f)
{
	m_rgrAITick.SetTickTime(0.1f);
	m_rgrAITick.Start();
}

GNPCMgr::~GNPCMgr(void)
{
	Clear();
}

void GNPCMgr::Clear()
{
	m_mapUIDNPC.clear();
	m_vecNPCs.clear();
	m_vecAddNPCs.clear();
	m_nLastUpdatedIndex = 0;
}

void GNPCMgr::AddNPC(GEntityNPC* pNPC)
{
	VALID(pNPC);
	m_mapUIDNPC[pNPC->GetUID()] = pNPC;
	m_vecAddNPCs.push_back(pNPC);

	//mlog("%s - %u\n", __FUNCTION__, pNPC);
}

void GNPCMgr::RemoveNPC(GEntityNPC* pNPC)
{
	VALID(pNPC);
	m_mapUIDNPC.erase(pNPC->GetUID());

	vector<GEntityNPC*>::iterator it =
		std::find(m_vecNPCs.begin(), m_vecNPCs.end(), pNPC);
//	VALID(it != m_vecNPCs.end());
	if (it == m_vecNPCs.end())
	{
		vector<GEntityNPC*>::iterator it =
			std::find(m_vecAddNPCs.begin(), m_vecAddNPCs.end(), pNPC);

		if (it != m_vecAddNPCs.end())
		{
			m_vecAddNPCs.erase(it);
		}

		return;
	}

	//mlog("%s - %u\n", __FUNCTION__, pNPC);

	m_vecNPCs.erase(it);
}

GEntityNPC* GNPCMgr::GetEntityNPC(MUID uid)
{
	return m_mapUIDNPC[uid];
}

void GNPCMgr::Update(float fDelta)
{
	// 이전 순회중에 추가된 NPC들 추가
	if (!m_vecAddNPCs.empty())
	{
//		for each (GEntityNPC* each in m_vecAddNPCs)
//			mlog("%s1 - %u\n", __FUNCTION__, each);

		m_vecNPCs.insert(m_vecNPCs.end(), m_vecAddNPCs.begin(), m_vecAddNPCs.end());
		m_vecAddNPCs.clear();
	}

	if (m_vecNPCs.empty())
		return;

	fTickElapsedTime += fDelta;

	if (m_rgrAITick.IsReady(fDelta))
	{
		static const size_t MAX_UPDATE_QTY=10000;

		size_t nLimitUpdateCount = min(MAX_UPDATE_QTY, m_vecNPCs.size());
		size_t nUpdatedCount = 0;

		if (m_nLastUpdatedIndex >= m_vecNPCs.size())
		{
			m_nLastUpdatedIndex = 0;
		}

		vector<GEntityNPC*>::iterator it = m_vecNPCs.begin() + m_nLastUpdatedIndex;
		
		for (; ; ++m_nLastUpdatedIndex)
		{
			if (it == m_vecNPCs.end())
			{
				it = m_vecNPCs.begin();
				m_nLastUpdatedIndex = 0;
			}

			GEntityNPC* pNPC = *it;
			//if (pNPC->IsDeleteMe())
			//{
			//	it = m_vecNPCs.erase(it);

			//	if (m_vecNPCs.empty())
			//		break;

			//	continue;
			//}

			pNPC->UpdateAI(fTickElapsedTime);

			if (++nUpdatedCount >= nLimitUpdateCount)
				break; // 이번틱 업데이트 횟수 완료

			++it;
		}

		fTickElapsedTime = 0.0f;
	}

	// 이번 순회중에 추가된 NPC들 추가
	if (!m_vecAddNPCs.empty())
	{
//		for each (GEntityNPC* each in m_vecAddNPCs)
//			mlog("%s2 - %u\n", __FUNCTION__, each);

		m_vecNPCs.insert(m_vecNPCs.end(), m_vecAddNPCs.begin(), m_vecAddNPCs.end());
		m_vecAddNPCs.clear();
	}
}

size_t GNPCMgr::GetSize() const
{
	return m_vecAddNPCs.size();
}