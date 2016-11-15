#include "stdafx.h"
#include "XDummyBot_Combat.h"
#include "XDummyMaster.h"
#include "XNPCInfo.h"
#include "XPost_GM.h"
#include "XDummySpawnMgr.h"

XDummyBot_Combat::XDummyBot_Combat(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo) 
: XDummyBot_Roam(pAgent, pAgentInfo), m_nTargetUIID(UIID_INVALID), m_nLastPostAttackTime(0)
{
	m_pSpawnMgr = new XDummySpawnMgr(pAgentInfo);

	m_fAttackTime = pAgentInfo->GetFloatValue(L"attack_time");
	if (m_fAttackTime == 0) m_fAttackTime = 2.0f;
}

XDummyBot_Combat::~XDummyBot_Combat()
{
	SAFE_DELETE(m_pSpawnMgr);
}

void XDummyBot_Combat::OnRun(float fDelta)
{
	m_pSpawnMgr->Update(fDelta, m_pAgent->GetClient(), m_vPosition);

	if (false == IsValidTarget())
	{
		Roam();
		SelectTarget();
	}
	else
	{
		m_vGoalPos = GetTargetPos();

		if (m_vGoalPos.DistanceTo(m_vPosition) < 100.0f)
		{
			PostAttack();
		}

		if (m_vGoalPos.DistanceTo(m_vPosition) < 100.0f) return;
	}

	UpdateMovingPost(fDelta);	

	static float m_stfElapsedTime = 0.0f;
	m_stfElapsedTime += fDelta;

	if (m_stfElapsedTime > 30.0f)
	{
		m_nTargetUIID = 0;
		m_stfElapsedTime = 0.0f;
	}
}

minet::MCommandResult XDummyBot_Combat::OnCommand( XBirdDummyAgent* pAgent, MCommand* pCommand )
{
	assert(NULL!=pAgent);
	assert(NULL!=pCommand);

	XDummyBot::OnCommand(pAgent, pCommand);

	XBirdClient* pClient = pAgent->GetClient();
	switch(pCommand->GetID())
	{
	case MC_CHAR_MYINFO:
		{
			TD_MYINFO vecTDMyInfo;
			vector<TD_ITEM>	vecTDInventory;
			vector<TD_ITEM> vecTDEquipment;

			if (pCommand->GetSingleBlob(vecTDMyInfo, 0)==false) return CR_ERROR;
			if (pCommand->GetBlob(vecTDInventory, 1)==false) return CR_ERROR;
			if (pCommand->GetBlob(vecTDEquipment, 2)==false) return CR_ERROR;

			//// 장비된 무기가 없으면, 인벤토리 아이템 장비 시도
			//bool isEquipWeapon = false;
			//for (size_t i = 0; i < vecTDEquipment.size(); i++)
			//{
			//	if (vecTDEquipment[i].m_nSlotID == ITEMSLOT_RWEAPON)
			//	{
			//		isEquipWeapon = true;
			//		break;
			//	}
			//}

			//if (!isEquipWeapon)
			//{
			//	for(size_t i = 0; i < vecTDInventory.size(); i++)
			//	{
			//		int nInvenSlotID = vecTDInventory[i].m_nSlotID;
			//		XBIRDPOSTCMD2(pClient, MC_ITEM_EQUIP_REQ, MCommandParameterInt(nInvenSlotID), MCommandParameterChar(char(ITEMSLOT_RWEAPON)));
			//	}
			//}

			if (3 != vecTDMyInfo.nPlayerGrade)
			{
				XBIRDPOSTCMD2(pClient, MC_GM_SET_ME_REQ, MCommandParameterWideString(L"grade"), MCommandParameterWideString(L"3"));
			}
			
			if (50 != vecTDMyInfo.nLevel)
			{
				XBIRDPOSTCMD0(pClient, MC_GM_GOD_REQ);
			}
		}
		break;
	case MC_FIELD_CHANGE_FIELD:
		{
			XBIRDPOSTCMD1(pClient, MC_ACTION_CHANGE_STANCE_REQ, MCommandParameterChar(1));
		}
		break;
	case MC_ACTION_TALENT_HIT:
		{
			MCommandParameter* pParam = pCommand->GetParameter(0);
			if(pParam->GetType()!=MPT_SINGLE_BLOB) return CR_FALSE;
			TD_TALENT_HIT* td_talent_hit = static_cast<TD_TALENT_HIT*>(pParam->GetPointer());

			if (td_talent_hit->nVictimUIID == m_pAgent->GetUIID())
			{
				UpdateTarget(td_talent_hit->nAttackerUIID);
			}
		}
		break;
	default:
		{
		}
		break;
	}

	m_pSpawnMgr->OnCommand(pCommand);
	if (XDummyBot_Roam::OnCommand(pAgent, pCommand) == CR_TRUE) return CR_TRUE;

	return CR_FALSE;
}

void XDummyBot_Combat::PostAttack()
{
	uint32 nNowTime = timeGetTime();
	if (nNowTime - m_nLastPostAttackTime < (uint32)(m_fAttackTime * 1000)) return;
	m_nLastPostAttackTime = nNowTime;

	TD_REQUEST_ATTACK ra;
	ra.vPos = m_vPosition;
	ra.vPos.z = m_pAgent->GetGroundZ(m_vPosition);

	vec3 vDir = m_vGoalPos - m_vPosition;
	vDir.z = 0.0f;

	vDir.Normalize();
	ra.svDir = vDir;

	ra.nAttackType = NA_NORMAL1;

	XBIRDPOSTCMD1(m_pAgent->GetClient(), MC_ACTION_ATTACK_MELEE_REQ, MCommandParameterSingleBlob(&ra,sizeof(TD_REQUEST_ATTACK)));
}

void XDummyBot_Combat::SelectTarget()
{	
	XNPCForDummy* pNPC = m_pSpawnMgr->GetRandomNearestNPC(m_vPosition);
	if (NULL == pNPC) return;

	m_nTargetUIID = pNPC->nUIID;
}

bool XDummyBot_Combat::IsValidTarget()
{
	XNPCForDummy* pNPC = m_pSpawnMgr->GetNPC(m_nTargetUIID);
	if (NULL == pNPC) return false;
	if (pNPC->bDead) return false;

	return true;
}

vec3 XDummyBot_Combat::GetTargetPos()
{
	XNPCForDummy* pNPC = m_pSpawnMgr->GetNPC(m_nTargetUIID);
	if (NULL == pNPC) return vec3::ZERO;

	return pNPC->vPos;
}

void XDummyBot_Combat::UpdateTarget( UIID nUIID )
{
	XNPCForDummy* pNPC = m_pSpawnMgr->GetNPC(nUIID);
	if (NULL == pNPC) return;

	m_nTargetUIID = nUIID;
}
