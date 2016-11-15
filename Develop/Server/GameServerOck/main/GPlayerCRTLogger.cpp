#include "StdAfx.h"
#include "GPlayerCRTLogger.h"
#include "GEntityPlayer.h"
#include "MCommand.h"
#include "GCommand.h"
#include "GConst.h"
#include "CCommandTable.h"

void GPlayerCRTLogger::Init()
{
	if (0 < GConst::LOG_CRT)
	{
		m_setLoggableCRT.insert(static_cast<CCommandResultTable>(GConst::LOG_CRT));
	}	
}

void GPlayerCRTLogger::Insert(GEntityPlayer* pPlayer, CCommandResultTable nCRT)
{
	if (NULL == pPlayer) return;

	m_setLoggableCRT.insert(nCRT);

	RouteLogCRTList(pPlayer);	
}

void GPlayerCRTLogger::Delete(GEntityPlayer* pPlayer, CCommandResultTable nCRT)
{
	if (NULL == pPlayer) return;

	m_setLoggableCRT.erase(nCRT);

	RouteLogCRTList(pPlayer);	
}

void GPlayerCRTLogger::Log(GEntityPlayer* pPlayer,CCommandResultTable nCRT)
{
	if (NULL == pPlayer) return;

	for each (CCommandResultTable nLoggableFail in m_setLoggableCRT)
	{
		if (nCRT < nLoggableFail)		continue;
		if (nCRT >= nLoggableFail+100)	continue;

		mlog3("FailCRT : Name(%s), UID(%I64d), CRT(%d)\n", MLocale::ConvUTF16ToAnsi(pPlayer->GetName()).c_str(), pPlayer->GetUID(),  nCRT);
	}
}

void GPlayerCRTLogger::RouteLogCRTList(GEntityPlayer* pPlayer)
{
	if (NULL == pPlayer) return;

	vector<int> vecLoggableCRT;
	for each (CCommandResultTable nCRT in m_setLoggableCRT)
	{
		vecLoggableCRT.push_back(static_cast<int>(nCRT));
	}

	MCommand* pNewCmd = MakeNewCommand(MC_GM_LOG_CRT_LIST,
		1,
		NEW_BLOB(vecLoggableCRT));

	pPlayer->RouteToMe(pNewCmd);
}