#include "StdAfx.h"
#include "XDummyPlayerMgr.h"
#include "CSDef_QuestPVP.h"

XPlayerForDummy::XPlayerForDummy(UIID nUIID, vec3 vPos, bool bDead, QUESTPVP_TEAM nQPVPTeam)
: nUIID(nUIID), vPos(vPos), bDead(bDead), nQPVPTeam(nQPVPTeam)
{
	
}

XDummyPlayerMgr::XDummyPlayerMgr(void)
{
}

XDummyPlayerMgr::~XDummyPlayerMgr(void)
{
	SAFE_DELETE_MAP(m_mapPlayerForDummy);
}

minet::MCommandResult XDummyPlayerMgr::OnCommand( MCommand* pCommand )
{
	switch(pCommand->GetID())
	{
	case MC_QUESTPVP_ENTER:
		{
			UIID nPlayerUIID;
			if ( pCommand->GetParameter( &nPlayerUIID, 0, MPT_USHORT) == false)		return CR_ERROR;
			int nTeam;
			if ( pCommand->GetParameter( &nTeam, 1, MPT_INT) == false)			return CR_ERROR;

			XPlayerForDummy* pPlayer = Get(nPlayerUIID);
			if (NULL != pPlayer)
			{
				pPlayer->nQPVPTeam = static_cast<QUESTPVP_TEAM>(nTeam);
			}
		}
		break;

	case MC_FIELD_IN_PLAYER:
		{
			MCommandParameter* pParam = pCommand->GetParameter(0);
			if(pParam->GetType()!=MPT_SINGLE_BLOB) return CR_ERROR;
			TD_UPDATE_CACHE_PLAYER* pPlayerInfo = (TD_UPDATE_CACHE_PLAYER*)pParam->GetPointer();

			bool bDead = false;
			if (CheckBitSet(pPlayerInfo->nStatusFlag, UPS_DEAD))
				bDead = true;

			QUESTPVP_TEAM nQPVPTeam = QT_NONE;
			if (CheckBitSet(pPlayerInfo->nStatusFlag, UPS_FIELDPVP_TEAM1))
			{
				nQPVPTeam = QT_ONE;
			}
			else if (CheckBitSet(pPlayerInfo->nStatusFlag, UPS_FIELDPVP_TEAM2))
			{
				nQPVPTeam = QT_TWO;
			}

			Insert(pPlayerInfo->nUIID, pPlayerInfo->vPos, bDead, nQPVPTeam);			
		}
		break;
	case MC_FIELD_OUT_PLAYER:
		{
			UIID nPlayerUIID;
			if (pCommand->GetParameter(&nPlayerUIID,	0, MPT_USHORT)==false) return CR_ERROR;

			Delete(nPlayerUIID);
		}
		break;
	case MC_FIELD_SECTOR_ENTITY_INFO:
		{
			vector<TD_UPDATE_CACHE_PLAYER> vecPlayerInfo;
			if (pCommand->GetBlob(vecPlayerInfo, 0)==false) return CR_FALSE;
			for each(const TD_UPDATE_CACHE_PLAYER& playerInfo in vecPlayerInfo)
			{
				bool bDead= false;
				if (CheckBitSet(playerInfo.nStatusFlag, UPS_DEAD))
					bDead = true;

				Insert(playerInfo.nUIID, playerInfo.vPos, bDead);
			}
		}
		break;
	case MC_ACTION_MOVE:
		{
			UIID nUIID;
			TD_PC_MOVE* pMove;
			if (pCommand->GetParameter(&nUIID,	0, MPT_USHORT)==false) return CR_ERROR;
			if (!pCommand->GetSingleBlob(pMove, 1))	return CR_ERROR;

			XPlayerForDummy* pPlayer = Get(nUIID);
			if (pPlayer)
			{
				pPlayer->vPos = pMove->vTarPos;
			}
		}
		break;
	case MC_ACTION_DIE:
		{
			UIID nVictimUIID;
			if ( pCommand->GetParameter( &nVictimUIID, 0, MPT_USHORT) == false) return CR_FALSE;

			XPlayerForDummy* pPlayer = Get(nVictimUIID);
			if (pPlayer)
			{
				pPlayer->bDead = true;
			}
		}
		break;
	case MC_ACTION_DESPAWN:
		{
			UIID nUIID;
			if ( pCommand->GetParameter( &nUIID, 0, MPT_USHORT) == false) return CR_FALSE;

			Delete(nUIID);
		}
		break;
	}

	return CR_FALSE;
}

void XDummyPlayerMgr::Insert( UIID nUIID, vec3 vPos, bool bDead, QUESTPVP_TEAM nQPVPTeam )
{
	if (NULL != Get(nUIID)) return;

	m_mapPlayerForDummy.insert(MAP_PLAYERFORDUMMY::value_type(nUIID, new XPlayerForDummy(nUIID, vPos, bDead, nQPVPTeam)));
}

void XDummyPlayerMgr::Delete( UIID nUIID )
{
	XPlayerForDummy* pPlayerForDummy = Get(nUIID);
	SAFE_DELETE(pPlayerForDummy);

	m_mapPlayerForDummy.erase(nUIID);
}

XPlayerForDummy* XDummyPlayerMgr::Get( UIID nUIID )
{
	MAP_PLAYERFORDUMMY::iterator itor = m_mapPlayerForDummy.find(nUIID);
	if (m_mapPlayerForDummy.end() == itor) return NULL;

	return itor->second;
}

XPlayerForDummy* XDummyPlayerMgr::GetEnemy(vec3 vPos, QUESTPVP_TEAM nQPVPTeam)
{
	XPlayerForDummy* pSelectedPlayer = NULL;;

	float fDist = FLT_MAX;
	for (MAP_PLAYERFORDUMMY::iterator itor = m_mapPlayerForDummy.begin(); itor != m_mapPlayerForDummy.end(); ++itor)
	{
		XPlayerForDummy* pPlayer = itor->second;

		if (pPlayer->bDead) continue;
		if (pPlayer->nQPVPTeam != nQPVPTeam) continue;
		if (pPlayer->vPos.DistanceToSq(vPos) >= fDist) continue;

		fDist = pPlayer->vPos.DistanceToSq(vPos);
		pSelectedPlayer = pPlayer;
	}

	return pSelectedPlayer;
}

