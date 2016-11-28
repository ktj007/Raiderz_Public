#include "StdAfx.h"
#include "GPlayerCRTLogger.h"
#include "GEntityPlayer.h"
#include "GConst.h"

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
}

void GPlayerCRTLogger::Delete(GEntityPlayer* pPlayer, CCommandResultTable nCRT)
{
	if (NULL == pPlayer) return;

	m_setLoggableCRT.erase(nCRT);
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
