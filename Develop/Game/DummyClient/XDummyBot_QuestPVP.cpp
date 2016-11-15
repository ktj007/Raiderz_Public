#include "StdAfx.h"
#include "XDummyBot_QuestPVP.h"
#include "XDummySpawnMgr.h"
#include "XBirdDummyAgent.h"
#include "XDummyPlayerMgr.h"
#include "CSDef_QuestPVP.h"
#include "CSFactionInfo.h"
#include "CSFactionCalculator.h"

XDummyBot_QuestPVP::XDummyBot_QuestPVP( XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo )
: XDummyBot_Combat(pAgent, pAgentInfo), m_nPhase(PHASE_INIT_FACTION), m_pPlayerMgr(NULL), m_bInitFactionInc(false), m_bInitFactionDec(false)
{
	m_pPlayerMgr = new XDummyPlayerMgr();

	m_nTeam = m_pAgent->IsOddNumberID() ? QT_ONE : QT_TWO;
}

XDummyBot_QuestPVP::~XDummyBot_QuestPVP(void)
{
	SAFE_DELETE(m_pPlayerMgr);
}

void XDummyBot_QuestPVP::OnRun( float fDelta )
{
	switch (m_nPhase)
	{
	case PHASE_INIT_FACTION:
		{
			int nIncFactionID = 0;
			int nDecFactionID = 0;
			if (QT_ONE == m_nTeam)
			{
				nIncFactionID = 115;
				nDecFactionID = 116;
			}
			else
			{
				nIncFactionID = 116;
				nDecFactionID = 115;
			}	

			XBIRDPOSTCMD0(m_pAgent->GetClient(), MC_GM_FACTION_RESET_REQ);
			XBIRDPOSTCMD2(m_pAgent->GetClient(), MC_GM_FACTION_INCREASE_REQ, MCommandParameterUChar(nIncFactionID), MCommandParameterUShort(CSFactionCalculator::GetMaxQuantity()));
			XBIRDPOSTCMD2(m_pAgent->GetClient(), MC_GM_FACTION_DECREASE_REQ, MCommandParameterUChar(nDecFactionID), MCommandParameterUShort(CSFactionCalculator::GetMaxQuantity()));
	
			ChangePhase(PHASE_INIT_FACTION_DONE);
		}
		break;	
	// 팩션이 변경될때 실시간으로 클라이언트에 PVPTeam 갱신이 아직 안되서, 섹터밖으로 나갔다 다시 들어오게 한다.
	case PHASE_REFRESH_FACTION:
		{
			XBIRDPOSTCMD2(m_pAgent->GetClient(), MC_GM_MOVE_REQ, MCommandParameterInt(m_pAgent->GetLoginFieldID()), MCommandParameterVector(vec3::ZERO));
			XBIRDPOSTCMD2(m_pAgent->GetClient(), MC_GM_MOVE_REQ, MCommandParameterInt(m_pAgent->GetLoginFieldID()), MCommandParameterVector(m_pAgent->GetCenter()));

			ChangePhase(PHASE_RUN);			
		}
		break;
	case PHASE_RUN:
		{
			__super::OnRun(fDelta);
		}
		break;
	}
}


minet::MCommandResult XDummyBot_QuestPVP::OnCommand( XBirdDummyAgent* pAgent, MCommand* pCommand )
{
	__super::OnCommand(pAgent, pCommand);
	m_pPlayerMgr->OnCommand(pCommand);

	XBirdClient* pClient = pAgent->GetClient();
	switch(pCommand->GetID())
	{
	case MC_FACTION_INCREASE:
		{
			uint8 nFactionID;
			uint16 nFactionQuantity;
			if (pCommand->GetParameter(&nFactionID,			0,	MPT_UCHAR)==false) return CR_ERROR;
			if (pCommand->GetParameter(&nFactionQuantity,	1,	MPT_USHORT)==false) return CR_ERROR;

			if (nFactionQuantity == CSFactionCalculator::GetMaxQuantity())
			{
				m_bInitFactionInc = true;
			}			

			if (m_bInitFactionInc && m_bInitFactionDec)
			{
				ChangePhase(PHASE_REFRESH_FACTION);
			}
		}
		break;
	case MC_FACTION_DECREASE:
		{
			uint8 nFactionID;
			uint16 nFactionQuantity;
			if (pCommand->GetParameter(&nFactionID,			0,	MPT_UCHAR)==false) return CR_ERROR;
			if (pCommand->GetParameter(&nFactionQuantity,	1,	MPT_USHORT)==false) return CR_ERROR;

			if (nFactionQuantity == CSFactionCalculator::GetMinQuantity())
			{
				m_bInitFactionDec = true;
			}
			
			if (m_bInitFactionInc && m_bInitFactionDec)
			{
				ChangePhase(PHASE_REFRESH_FACTION);
			}
		}
		break;
	}

	return CR_FALSE;
}

void XDummyBot_QuestPVP::SelectTarget()
{
	if (true == XMath::RandomBool())
	{
		int nFactionID = 0;

		if (QT_ONE == m_nTeam)
		{
			nFactionID = 116;
		}
		else if (QT_TWO == m_nTeam)
		{
			nFactionID = 115;
		}

		XNPCForDummy* pNPC = m_pSpawnMgr->GetNearestNPC(m_vPosition, false, nFactionID);
		if (NULL != pNPC)
		{
			m_nTargetUIID = pNPC->nUIID;
		}
	}
	else
	{
		QUESTPVP_TEAM nTeam = QT_NONE;

		if (QT_ONE == m_nTeam)
		{
			nTeam = QT_TWO;			 
		}
		else if (QT_TWO == m_nTeam)
		{
			nTeam = QT_ONE;
		}

		XPlayerForDummy* pPlayer = m_pPlayerMgr->GetEnemy(m_vPosition, nTeam);
		if (NULL != pPlayer)
		{
			m_nTargetUIID = pPlayer->nUIID;
		}
	}
}

bool XDummyBot_QuestPVP::IsValidTarget()
{
	if (true == __super::IsValidTarget())
	{
		return true;
	}

	XPlayerForDummy* pPlayer = m_pPlayerMgr->Get(m_nTargetUIID);
	if (NULL == pPlayer) return false;
	if (pPlayer->bDead) return false;

	return true;
}

vec3 XDummyBot_QuestPVP::GetTargetPos()
{
	vec3 vPos = __super::GetTargetPos();
	if (vec3::ZERO != vPos) return vPos;

	XPlayerForDummy* pPlayer = m_pPlayerMgr->Get(m_nTargetUIID);
	if (NULL == pPlayer) return vec3::ZERO;

	return pPlayer->vPos;
}

void XDummyBot_QuestPVP::UpdateTarget( UIID nUIID )
{
	__super::UpdateTarget(nUIID);

	XPlayerForDummy* pPlayer = m_pPlayerMgr->Get(m_nTargetUIID);
	if (NULL == pPlayer) return;

	m_nTargetUIID = nUIID;

}

void XDummyBot_QuestPVP::ChangePhase( PHASE nPhase )
{
	m_nPhase = nPhase;
}

