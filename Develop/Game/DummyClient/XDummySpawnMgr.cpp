#include "stdafx.h"
#include "XDummySpawnMgr.h"
#include "XDummyMaster.h"
#include "XNPCInfo.h"

XNPCForDummy::XNPCForDummy()
{
	UID = MUID::ZERO;
	nLastLiveTime = timeGetTime();
	bDead = false;
	pNPCInfo = NULL;
}

void XNPCForDummy::Touch()
{
	nLastLiveTime = timeGetTime();
}

XDummySpawnMgr::XDummySpawnMgr( XDummyAgentInfo* pAgentInfo, bool bOptimize)
: m_bAutoSpawn(false), m_fSpawnTick(1.0f), m_nMaxSpawn(0), m_bOptimize(bOptimize)
{
	Init(pAgentInfo);
}

void XDummySpawnMgr::Init(XDummyAgentInfo* pAgentInfo)
{
	wstring strautospawn = pAgentInfo->GetValue(L"auto_spawn");

	if (!strautospawn.empty())
	{
		if (!wcsicmp(strautospawn.c_str(), L"true"))
		{
			m_bAutoSpawn = true;
		}
	}

	wstring strspawntick = pAgentInfo->GetValue(L"spawn_tick");
	if (!strspawntick.empty())
	{
		int nticktime = _wtoi(strspawntick.c_str());
		if (nticktime > 0)
		{
			m_fSpawnTick = (float)nticktime;
		}
	}

	wstring strmaxspawn = pAgentInfo->GetValue(L"max_spawn");
	if (!strmaxspawn.empty())
	{
		int nvalue = _wtoi(strmaxspawn.c_str());
		if (nvalue > 0)
		{
			m_nMaxSpawn = nvalue;
		}
	}

	for ( multimap< wstring, wstring>::iterator itor = pAgentInfo->mapValues.begin();  itor != pAgentInfo->mapValues.end();  itor++)
	{
		wstring strkey = (*itor).first;
		wstring strvalue = (*itor).second;

		if (strkey == L"spawn_id")
		{
			int nid = _wtoi(strvalue.c_str());
			if (nid != 0)
			{
				m_vecNPCID.push_back(nid);
			}
		}
	}

	if (m_bAutoSpawn)
	{
		m_SpawnRegulator.SetTickTime(m_fSpawnTick);
		m_SpawnRegulator.Start();
	}	

	m_RemoveInvalidNPCRegulator.SetTickTime(1.0f);
	m_RemoveInvalidNPCRegulator.Start();

}

void XDummySpawnMgr::Update(float fDelta, XBirdClient* pClient, vec3 vPosition)
{
	RemoveInvalidNPC(fDelta, vPosition);
	Spawn(fDelta, pClient, vPosition);
}

void XDummySpawnMgr::RemoveInvalidNPC(float fDelta, vec3 vPosition)
{
	if (m_RemoveInvalidNPCRegulator.IsReady(fDelta) == false) return;

	uint32 nNowTime = timeGetTime();

	for (XNPCForDummyMap::iterator itor = m_mapNPCForDummy.begin(); itor != m_mapNPCForDummy.end(); )
	{
		XNPCForDummy* npc = (*itor).second;
		if (npc->vPos.DistanceTo(vPosition) > 10000 ||
			(m_bOptimize && nNowTime - npc->nLastLiveTime > 60000))
		{
			itor = m_mapNPCForDummy.erase(itor);
		}
		else
		{
			itor++;
		}
	}
}

void XDummySpawnMgr::Spawn(float fDelta, XBirdClient* pClient, vec3 vPosition)
{
	if (NULL == pClient) return;

	if (m_bAutoSpawn == false) return;

	if (m_SpawnRegulator.IsReady(fDelta) == false) return;
	if (m_nMaxSpawn <= static_cast<int>(m_mapNPCForDummy.size())) return;

	int nNpcID = 0;
	if (false == m_vecNPCID.empty())
	{
		int nIndex = XMath::RandomNumber(0, static_cast<int>(m_vecNPCID.size()) - 1);
		nNpcID = m_vecNPCID[nIndex];
	}
	else
	{
		nNpcID = RandomMonsterID();
	}

	if (nNpcID > 0)
	{
		XBIRDPOSTCMD3(pClient, 
			MC_GM_REQUEST_SPAWN, 
			MCommandParameterInt(nNpcID), 
			MCommandParameterInt(1), 
			MCommandParameterVector(vPosition + vec3(1000.0f, 0.0f, 0.0f)));
	}
}


int XDummySpawnMgr::RandomMonsterID()
{
	int nSize = (int)info.npc->size();
	if (nSize <= 0) return 0;

	int nCnt = 0;

	while (nCnt < 100)
	{
		nCnt++;

		int nIndex = XMath::RandomNumber(0, nSize - 1);

		XNPCInfoMgr::iterator itor = info.npc->begin();
		for (int i = 0; i < nIndex; i++)
		{
			itor++;
			if (itor == info.npc->end()) return 0;
		}

		CSNPCInfo* pNPCInfo = (*itor).second;

		if (pNPCInfo->bValidation == false) continue;

		if (pNPCInfo->nNpcType == NPC_TYPE_MONSTER)
		{
			return pNPCInfo->nID;
		}
	}

	return 0;
}

void XDummySpawnMgr::InsertNPC( XNPCForDummy* npc )
{
	m_mapNPCForDummy.insert(XNPCForDummyMap::value_type(npc->nUIID, npc));
}

void XDummySpawnMgr::DeleteNPC( UIID nUIID )
{
	XNPCForDummyMap::iterator itor = m_mapNPCForDummy.find(nUIID);
	if (itor != m_mapNPCForDummy.end())
	{
		delete (*itor).second;
		m_mapNPCForDummy.erase(itor);
	}
}

XNPCForDummy* XDummySpawnMgr::GetNPC( UIID nUIID )
{
	XNPCForDummyMap::iterator itor = m_mapNPCForDummy.find(nUIID);
	if (itor != m_mapNPCForDummy.end())
	{
		return (*itor).second;
	}
	return NULL;
}

XNPCForDummy* XDummySpawnMgr::GetNPC( MUID nUID )
{
	for each (const XNPCForDummyMap::value_type& val in m_mapNPCForDummy)
	{
		XNPCForDummy* pNPC = val.second;
		if (nUID == pNPC->UID)
		{
			return pNPC;
		}
	}

	return NULL;
}

XNPCForDummy* XDummySpawnMgr::GetNearestNPC( vec3 vPos, bool bDead, int nFactionID)
{
	float fDist = FLT_MAX;
	XNPCForDummy* pNPC = NULL;

	for (XNPCForDummyMap::iterator itor = m_mapNPCForDummy.begin(); itor != m_mapNPCForDummy.end(); ++itor)
	{
		XNPCForDummy* npc = (*itor).second;

		if (bDead != npc->bDead) continue;
		if (NAT_NONE == npc->pNPCInfo->nInitAttackable) continue;
		if (NAT_FACTION == npc->pNPCInfo->nInitAttackable &&
			0 != nFactionID && 0 != npc->pNPCInfo->m_nFactionLossID)
		{
			if (nFactionID != npc->pNPCInfo->m_nFactionLossID ) continue;
		}		

		vec3 vNPCPos = npc->vPos;

		if (vNPCPos.DistanceToSq(vPos) < fDist)
		{
			fDist = vNPCPos.DistanceToSq(vPos);
			pNPC = npc;
		}
	}

	return pNPC;
}

XNPCForDummy* XDummySpawnMgr::GetRandomNearestNPC( vec3 vPos, bool bDead, int nFactionID)
{
	XNPCForDummy* pNPC = NULL;

	for (XNPCForDummyMap::iterator itor = m_mapNPCForDummy.begin(); itor != m_mapNPCForDummy.end(); ++itor)
	{
		XNPCForDummy* npc = (*itor).second;

		if (bDead != npc->bDead) continue;
		if (NAT_NONE == npc->pNPCInfo->nInitAttackable) continue;
		if (NAT_FACTION == npc->pNPCInfo->nInitAttackable &&
			0 != nFactionID && 0 != npc->pNPCInfo->m_nFactionLossID)
		{
			if (nFactionID != npc->pNPCInfo->m_nFactionLossID ) continue;
		}		

		vec3 vNPCPos = npc->vPos;

		if (pNPC == NULL || XMath::RandomNumber(0, 10) <= 2)
		{
			pNPC = npc;
		}
	}

	return pNPC;
}

minet::MCommandResult XDummySpawnMgr::OnCommand( MCommand* pCommand )
{
	switch(pCommand->GetID())
	{
	case MC_FIELD_IN_NPC:
		{
			MCommandParameter* pParam = pCommand->GetParameter(0);
			if(pParam->GetType()!=MPT_SINGLE_BLOB) return CR_ERROR;
			TD_UPDATE_CACHE_NPC* pNPCInfo = (TD_UPDATE_CACHE_NPC*)pParam->GetPointer();

			XNPCInfo* pInfo = info.npc->Get(pNPCInfo->nNPCID);

			XNPCForDummy* npc = new XNPCForDummy();
			npc->UID = pNPCInfo->uid;
			npc->nUIID = pNPCInfo->nUIID;
			npc->vPos = pNPCInfo->vPos;
			npc->pNPCInfo = pInfo;

			if (CheckBitSet(pNPCInfo->nStatusFlag, UNS_DEAD))
				npc->bDead = true;

			InsertNPC(npc);
		}
		break;
	case MC_FIELD_OUT_NPC:
		{
			UIID nNPCUIID;
			if (pCommand->GetParameter(&nNPCUIID,	0, MPT_USHORT)==false) return CR_ERROR;

			DeleteNPC(nNPCUIID);
		}
		break;
	case MC_FIELD_SECTOR_ENTITY_INFO:
		{
			MCommandParameter* pParam = pCommand->GetParameter(1);
			if(pParam->GetType() != MPT_BLOB) return CR_ERROR;

			TD_UPDATE_CACHE_NPC* pNpcNode = (TD_UPDATE_CACHE_NPC*)pParam->GetPointer();
			int nNpcCount = pParam->GetBlobCount();

			for (int i = 0; i < nNpcCount; i++)
			{
				XNPCInfo* pInfo = info.npc->Get(pNpcNode[i].nNPCID);
				XNPCForDummy* npc = new XNPCForDummy();
				npc->UID = pNpcNode[i].uid;
				npc->nUIID = pNpcNode[i].nUIID;
				npc->vPos = pNpcNode[i].vPos;
				npc->pNPCInfo = pInfo;

				if (CheckBitSet(pNpcNode[i].nStatusFlag, UNS_DEAD))
					npc->bDead = true;

				InsertNPC(npc);
			}
		}
		break;
	case MC_NPC_MOVE:
		{
			MCommandParameter* pParam = pCommand->GetParameter(0);
			if(pParam->GetType()!=MPT_SINGLE_BLOB) return CR_FALSE;
			TD_NPC_MOVE* pNM = (TD_NPC_MOVE*)pParam->GetPointer();

			XNPCForDummy* pNPC = GetNPC(pNM->nUIID);
			if (pNPC)
			{
				pNPC->vPos = pNM->vTarPos;
				pNPC->Touch();
			}
		}
		break;
	case MC_NPC_RUN:
		{
			MCommandParameter* pParam = pCommand->GetParameter(0);
			if(pParam->GetType()!=MPT_SINGLE_BLOB) return CR_FALSE;
			TD_NPC_MOVE* pNM = (TD_NPC_MOVE*)pParam->GetPointer();

			XNPCForDummy* pNPC = GetNPC(pNM->nUIID);
			if (pNPC)
			{
				pNPC->vPos = pNM->vTarPos;
				pNPC->Touch();
			}
		}
		break;
	case MC_ACTION_DIE:
		{
			UIID nVictimUIID;
			if ( pCommand->GetParameter( &nVictimUIID, 0, MPT_USHORT) == false) return CR_FALSE;

			XNPCForDummy* pNPC = GetNPC(nVictimUIID);
			if (pNPC)
			{
				pNPC->bDead = true;
			}
		}
		break;
	}

	return CR_FALSE;
}