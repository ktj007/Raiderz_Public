#include "StdAfx.h"
#include "GQPTeamCountSender.h"
#include "GField.h"
#include "GConst.h"
#include "GEntityPlayer.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "CSDef_QuestPVP.h"
#include "GQuestPVP.h"

GQPTeamCountSender::GQPTeamCountSender(GField* pField)
: m_pField(pField)
{
	m_Regulator.SetTickTime(GConst::QUESTPVP_INFO_SENDTIME);
	m_Regulator.Start();	
}

GQPTeamCountSender::~GQPTeamCountSender(void)
{
}

void GQPTeamCountSender::Update( float fDelta )
{
	if (false == m_Regulator.IsReady(fDelta)) return;

	SendTeamCount();
}

void GQPTeamCountSender::SendTeamCount()
{
	vector<GEntityPlayer*> vecTeamOnePlayer = m_pField->GetQuestPVP()->CollectTeam(QT_ONE);
	vector<GEntityPlayer*> vecTeamTwoPlayer = m_pField->GetQuestPVP()->CollectTeam(QT_TWO);

	for each (GEntityPlayer* pPlayer in vecTeamOnePlayer)
	{
		MCommand* pNewCmd = MakeNewCommand(MC_QUESTPVP_TEAMCOUNT, 2, 
			NEW_INT(vecTeamOnePlayer.size()), 
			NEW_INT(vecTeamTwoPlayer.size()));

		pPlayer->RouteToMe(pNewCmd);		
	}

	for each (GEntityPlayer* pPlayer in vecTeamTwoPlayer)
	{
		MCommand* pNewCmd = MakeNewCommand(MC_QUESTPVP_TEAMCOUNT, 2, 
			NEW_INT(vecTeamOnePlayer.size()), 
			NEW_INT(vecTeamTwoPlayer.size()));

		pPlayer->RouteToMe(pNewCmd);		
	}
}