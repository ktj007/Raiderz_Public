#include "StdAfx.h"
#include "XDummyBot_Loot.h"
#include "XDummySpawnMgr.h"
#include "XBirdDummyAgent.h"
#include "XDummyLazyCommandPoster.h"

#define PHASE_LOCK_TIME 5.0f

XDummyBot_Loot::XDummyBot_Loot(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo)
: XDummyBot_Roam(pAgent, pAgentInfo), m_nPhase(PHASE_INIT_REPEAT), m_nTargetUIID(UIID_INVALID), m_nTargetUID(MUID::ZERO)
{
	m_pSpawnMgr = new XDummySpawnMgr(pAgentInfo);
	m_pPhaseRegulator = new MRegulator();
	m_pPhaseRegulator->SetTickTime(PHASE_LOCK_TIME);
}

XDummyBot_Loot::~XDummyBot_Loot(void)
{
	SAFE_DELETE(m_pSpawnMgr);
	SAFE_DELETE(m_pPhaseRegulator);
}

void XDummyBot_Loot::OnRun(float fDelta)
{
	m_pSpawnMgr->Update(fDelta, m_pAgent->GetClient(), m_vPosition);

	switch (m_nPhase)
	{	
	case PHASE_INIT_REPEAT:
		{
			XNPCForDummy* pNPC = m_pSpawnMgr->GetNearestNPC(m_vPosition);
			if (NULL != pNPC)
			{
				m_nTargetUIID = pNPC->nUIID;
				m_nTargetUID  = pNPC->UID;
				vec3 vPos = pNPC->vPos;

				PostMove(vPos);
				XLAZYCMDPOST1(m_pAgent->GetClient(), MC_GM_KILL_ENTITY_REQ, MCommandParameterUShort(m_nTargetUIID));
				ChangePhase(PHASE_INIT_DONE);
				m_pSpawnMgr->DeleteNPC(pNPC->nUIID);
			}
			else
			{
				Roam();
			}
		}
		break;
	case PHASE_END:
		{
			ChangePhase(PHASE_INIT_REPEAT);
		}
		break;
	}

	if (true == m_pPhaseRegulator->IsReady(fDelta))
	{
		ChangePhase(PHASE_END);
	}
}

minet::MCommandResult XDummyBot_Loot::OnCommand( XBirdDummyAgent* pAgent, MCommand* pCommand )
{
	__super::OnCommand(pAgent, pCommand);
	m_pSpawnMgr->OnCommand(pCommand);

	XBirdClient* pClient = pAgent->GetClient();
	switch(pCommand->GetID())
	{
	case MC_FIELD_CHANGE_FIELD:
		{
			XBIRDPOSTCMD2(pClient, MC_GM_SET_ME_REQ, MCommandParameterWideString(L"grade"), MCommandParameterWideString(L"3"));
			XBIRDPOSTCMD0(pClient, MC_GM_GOD_REQ);
		}
		break;
	case MC_ACTION_DIE:
		{
			UIID nVictimUIID;
			MUID nAttackerUID;
			if ( pCommand->GetParameter(&nVictimUIID, 0, MPT_USHORT) == false)		return CR_ERROR;
			if ( pCommand->GetParameter(&nAttackerUID, 1, MPT_UID) == false)		return CR_ERROR;

			m_pSpawnMgr->DeleteNPC(nVictimUIID);

			if (m_nTargetUIID == nVictimUIID)
			{
				if (nAttackerUID == m_pAgent->GetUID())
				{
					XLAZYCMDPOST1(pClient, MC_LOOT_BEGIN_REQ, MCommandParameterUID(m_nTargetUID));
					ChangePhase(PHASE_KILL);
				}
				else
				{
					ChangePhase(PHASE_END);
				}
			}
		}
		break;
	case MC_ACTION_DESPAWN:
		{
			UIID nVictimUIID;
			if ( pCommand->GetParameter(&nVictimUIID, 0, MPT_USHORT) == false)		return CR_ERROR;

			m_pSpawnMgr->DeleteNPC(nVictimUIID);
		}
		break;
	case MC_LOOT_REMOVE_LOOTABLE_NPC:
		{
			MUID nVictimUID;

			if ( pCommand->GetParameter(&nVictimUID, 0, MPT_UID) == false)		return CR_ERROR;

			if (m_nTargetUID == nVictimUID)
				ChangePhase(PHASE_END);
		}
		break;
	case MC_LOOT_BEGIN:
		{
			MUID uidLooter;
			if (pCommand->GetParameter(&uidLooter,	0, MPT_UID)==false)		return CR_ERROR;
			if (uidLooter != pAgent->GetUID()) return CR_TRUE;

			XLAZYCMDPOST1(pClient, MC_LOOT_SEE_ITEM_REQ, MCommandParameterUID(m_nTargetUID));
			ChangePhase(PHASE_BEGIN);			
		}
		break;
	case MC_LOOT_SEE_ITEM:
		{
			m_vecTDDropItem.clear();
			if (pCommand->GetBlob(m_vecTDDropItem,	0)==false)	return CR_ERROR;

			vector<MUID> vecLootItemUID = GetRandomLootItem();

			XLAZYCMDPOST2(pClient, MC_LOOT_LOOT_ITEM_REQ, MCommandParameterUID(m_nTargetUID), MCommandParameterBlob(vecLootItemUID));
			ChangePhase(PHASE_SHOW);			
		}
		break;
	case MC_LOOT_REFRESH_ITEM:
		{
			m_vecTDDropItem.clear();
			if (pCommand->GetBlob(m_vecTDDropItem,	0)==false)	return CR_ERROR;

			vector<MUID> vecLootItemUID = GetRandomLootItem();
			XLAZYCMDPOST2(pClient, MC_LOOT_LOOT_ITEM_REQ, MCommandParameterUID(m_nTargetUID), MCommandParameterBlob(vecLootItemUID));

			ChangePhase(PHASE_REFRESHITEM);
		}
		break;
	case MC_LOOT_END:
		{
			MUID uidLooter;
			if (pCommand->GetParameter(&uidLooter,	0, MPT_UID)==false)		return CR_ERROR;
			if (uidLooter != pAgent->GetUID()) return CR_TRUE;

			XLAZYCMDPOST0(pClient, MC_GM_CLEAR_INVENTORY_REQ);
			ChangePhase(PHASE_END);
		}
		break;

	case MC_ACTION_TALENT_HIT:
		{
			TD_TALENT_HIT talentHit;
			if (pCommand->GetSingleBlob(talentHit, 0)==false) return CR_ERROR;
			
			if (gvar.MyInfo.GetUIID() == talentHit.nVictimUIID)
			{
				ChangePhase(PHASE_END);
			}
		}
		break;
	case MC_ACTION_TALENT_HIT_NO_MF:
		{
			UIID nVictimUIID;
			if (pCommand->GetParameter(&nVictimUIID,	0, MPT_USHORT)==false) return CR_ERROR;

			if (gvar.MyInfo.GetUIID() == nVictimUIID)
			{
				ChangePhase(PHASE_END);
			}
		}
		break;
	case MC_LOCAL_TEST_SELF_MOTION_FACTOR:
		{
			TD_TALENT_HIT talentHit;
			if (pCommand->GetSingleBlob(talentHit, 0)==false) return CR_ERROR;

			if (gvar.MyInfo.GetUIID() == talentHit.nVictimUIID)
			{
				ChangePhase(PHASE_END);
			}
		}
		break;
	case MC_ACTION_BUFF_HIT_NO_MF:
		{
			UIID nEntityUIID;
			if (pCommand->GetParameter(&nEntityUIID,	0, MPT_USHORT)==false) return CR_ERROR;

			if (gvar.MyInfo.GetUIID() == nEntityUIID)
			{
				ChangePhase(PHASE_END);
			}
		}
		break;
	default:
		{

		}
		break;
	}

	return CR_TRUE;
}

void XDummyBot_Loot::ChangePhase( PHASE nPhase )
{
	m_nPhase = nPhase;

	m_pPhaseRegulator->Reset(false);
}

vector<MUID> XDummyBot_Loot::GetRandomLootItem()
{
	if (m_vecTDDropItem.empty()) return vector<MUID>();

	int nIndex = RandomNumber(0, m_vecTDDropItem.size()-1);

	MUID nSelctedItemUID = m_vecTDDropItem[nIndex].m_nDropItemUID;
	vector<MUID> vecSelctedItemUID;
	vecSelctedItemUID.push_back(nSelctedItemUID);

	return vecSelctedItemUID;
}

