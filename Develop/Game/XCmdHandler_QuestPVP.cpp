#include "StdAfx.h"
#include "XCmdHandler_QuestPVP.h"
#include "XGameState.h"
#include "XPvPAreaLogic.h"
#include "XPlayerPVP.h"
#include "CSDef_QuestPVP.h"
#include "XMyPlayer.h"
#include "XUIMiniMap.h"

XCmdHandler_QuestPVP::XCmdHandler_QuestPVP(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_QUESTPVP_TEAMCOUNT,		OnTeamCount);
	SetCmdHandler(MC_QUESTPVP_ENTER,			OnEnterPvpZone);
	SetCmdHandler(MC_QUESTPVP_BEGIN_EVENT,		OnBeginEvent);
	SetCmdHandler(MC_QUESTPVP_END_EVENT,		OnEndEvent);
	SetCmdHandler(MC_QUESTPVP_MARK_NPCPOS,		OnMarkNPCPos);
	SetCmdHandler(MC_QUESTPVP_UNMARK_NPCPOS,	OnUnmarkNPCPos);
}

MCommandResult XCmdHandler_QuestPVP::OnTeamCount(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nTeamOneCount;
	int nTeamTwoCount;
	if (pCommand->GetParameter(&nTeamOneCount,	0, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nTeamTwoCount,	1, MPT_INT)==false) return CR_ERROR;

	if ( global.script)
	{
		int nFriendly;
		int nEnemy;

		XPlayerPVP::XPvPAreaTeam _team = gvar.Game.pMyPlayer->GetPlayerPVP()->GetPvPAreaTeam();
		if ( _team == XPlayerPVP::ePvPAreaTeam_1)
		{
			nFriendly = nTeamOneCount;
			nEnemy = nTeamTwoCount;
		}
		else if ( _team == XPlayerPVP::ePvPAreaTeam_2)
		{
			nFriendly = nTeamTwoCount;
			nEnemy = nTeamOneCount;
		}
		else
		{
			return CR_TRUE;
		}


		global.script->GetGlueGameEvent().OnGameEvent( "MISSIONPROG", "QUESTPVP", nFriendly, nEnemy);
	}

	return CR_TRUE;
}


MCommandResult XCmdHandler_QuestPVP::OnEnterPvpZone(MCommand* pCommand, MCommandHandler* pHandler)
{
 	UIID nPlayerUIID;
	if ( pCommand->GetParameter( &nPlayerUIID, 0, MPT_USHORT) == false)		return CR_ERROR;

	int nTeam;
 	if ( pCommand->GetParameter( &nTeam, 1, MPT_INT) == false)			return CR_ERROR;


	XNetPlayer* pNetPlayer = gg.omgr->FindNetPlayer( nPlayerUIID);
	if ( pNetPlayer != NULL)
	{
		bool bFaction1 = false, bFaction2 = false;
		switch ( static_cast<QUESTPVP_TEAM>( nTeam))
		{
		case QT_NONE :
			break;

		case QT_ONE :
			bFaction1 = true;
			break;

		case QT_TWO :
			bFaction2 = true;
			break;
		}

		pNetPlayer->GetPlayerPVP()->SetPvPAreaFaction( bFaction1, bFaction2);

		if ( gg.currentgamestate)
		{
			XPvPAreaLogic* pPvPAreaLogic = gg.currentgamestate->GetPvPArea();
			if ( pPvPAreaLogic != NULL)
				pPvPAreaLogic->RefreshNetPlayers( pPvPAreaLogic->IsInPvPArea());
		}
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_QuestPVP::OnBeginEvent(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nEventID;
	if (pCommand->GetParameter(&nEventID,	0, MPT_INT)==false) return CR_ERROR;

	return CR_TRUE;
}

MCommandResult XCmdHandler_QuestPVP::OnEndEvent(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nEventID;
	int nWinnerTeam;
	if (pCommand->GetParameter(&nEventID,		0, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nWinnerTeam,	1, MPT_INT)==false) return CR_ERROR;

	switch ( static_cast<QUESTPVP_TEAM>( nWinnerTeam))
	{
	case QT_NONE :
		break;

	case QT_ONE :
		break;

	case QT_TWO :
		break;
	}

	XUIMiniMap::SetMissionTarget( 0);

	return CR_TRUE;
}

MCommandResult XCmdHandler_QuestPVP::OnMarkNPCPos(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nNPCID;
	vec3 vPos;
	if (pCommand->GetParameter(&nNPCID,		0, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&vPos,		1, MPT_VEC)==false) return CR_ERROR;

	XUIMiniMap::SetMissionTargetPos( &vPos);
	XUIMiniMap::SetMissionTarget( nNPCID);

	return CR_TRUE;
}

MCommandResult XCmdHandler_QuestPVP::OnUnmarkNPCPos(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nNPCID;
	if (pCommand->GetParameter(&nNPCID,		0, MPT_INT)==false) return CR_ERROR;

	XUIMiniMap::SetMissionTarget( 0);

	return CR_TRUE;
}