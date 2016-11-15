#include "StdAfx.h"
#include "GClientCharRouter.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GGlobal.h"
#include "GCommandCenter.h"
#include "CTransData.h"
#include "GClientCharRouter.h"


GClientCharRouter::GClientCharRouter(void)
{
	// do nothing
}

GClientCharRouter::~GClientCharRouter(void)
{
	// do nothing
}


void GClientCharRouter::CharMyTimeInfo(MUID uidPlayer
									, vector<TD_BUFF_REMAIN_TIME>& rvecTDBuffRemainTime
									, vector<TD_TALENT_REMAIN_COOLTIME>& rvecTDTalentRemainCoolTime
									, float fAutoRebirthRemainTime)
{
	MCommand* pNewCmd = MakeNewCommand(MC_CHAR_MYTIMEINFO, uidPlayer,
		3,
		NEW_BLOB(rvecTDBuffRemainTime),
		NEW_BLOB(rvecTDTalentRemainCoolTime),
		NEW_FLOAT(fAutoRebirthRemainTime)
		);
	gsys.pCommandCenter->PostCommand(pNewCmd);
}