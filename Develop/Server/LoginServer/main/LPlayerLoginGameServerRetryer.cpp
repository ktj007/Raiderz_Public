#include "StdAfx.h"
#include "LPlayerLoginGameServerRetryer.h"
#include "LPlayerLogic.h"


const float LPlayerLoginGameServerRetryer::TIME_TO_UPDATE_SEC = 3.0f;

LPlayerLoginGameServerRetryer::LPlayerLoginGameServerRetryer()
{
	m_rgltRetry.SetTickTime(TIME_TO_UPDATE_SEC);

	m_pPlayerLogic = new LPlayerLogic;
}

LPlayerLoginGameServerRetryer::~LPlayerLoginGameServerRetryer()
{
	SAFE_DELETE(m_pPlayerLogic);
}

void LPlayerLoginGameServerRetryer::Update(float fDelta)
{
	if (!m_rgltRetry.IsReady(fDelta)) return;
	if (m_listRetryer.empty()) return;

	LRetryerList::iterator itr = m_listRetryer.begin();
	for (; itr != m_listRetryer.end();)
	{
		RETRYER_NODE& refNode = (*itr);

		m_pPlayerLogic->SelMyChar(refNode.uidPlayer, refNode.nCharIndex);
		
		itr = m_listRetryer.erase(itr);
	}
}

void LPlayerLoginGameServerRetryer::AddRetryer(MUID uidPlayer, int nCharIndex)
{
	RETRYER_NODE nodeRetryer;
	nodeRetryer.uidPlayer = uidPlayer;
	nodeRetryer.nCharIndex = nCharIndex;

	m_listRetryer.push_back(nodeRetryer);
}

void LPlayerLoginGameServerRetryer::DelRetryer(MUID uidPlayer)
{
	LRetryerList::iterator itr = m_listRetryer.begin();
	for (; itr != m_listRetryer.end(); ++itr)
	{
		RETRYER_NODE& refNode = (*itr);

		if (uidPlayer == refNode.uidPlayer)
		{
			m_listRetryer.erase(itr);
			return;
		}
	}
}