#include "StdAfx.h"
#include "GPlayerLoot.h"
#include "GEntityNPC.h"
#include "GEntityPlayer.h"
#include "GPlayerDoing.h"
#include "GLootSystem.h"
#include "MTime.h"
#include "GGlobal.h"
#include "GNPCLoot.h"
#include "GDropList.h"
#include "GLootShower.h"
#include "GLootEnder.h"

GPlayerLoot::GPlayerLoot(GEntityPlayer* pOwner)
: m_pOwner(pOwner)
, m_nLootID(INVALID_ID)
{
	m_pILootRegulator = new MRegulator();

	m_pILootRegulator->Stop();	
}

GPlayerLoot::~GPlayerLoot()
{
	SAFE_DELETE(m_pILootRegulator);
}

void GPlayerLoot::Update( float fDelta )
{
	PFC_THISFUNC;
	if (false == UpdateInteractionLoot(fDelta))
	{
		gsys.pLootSystem->GetEnder().End(m_pOwner);
	}
}

bool GPlayerLoot::UpdateInteractionLoot(float fDelta)
{
	if (true == m_pILootRegulator->IsReady(fDelta))
	{
		m_pILootRegulator->Stop();

		GEntityNPC* pLootingNPC = GetLootingNPC();
		if(NULL == pLootingNPC) return false;

		if (pLootingNPC->GetNPCInfo()->nNpcType == NPC_TYPE_GATHER)
		{
			if (false == gsys.pLootSystem->PrepareGather(m_pOwner, pLootingNPC, m_nLootID)) return false;
		}

		if (false == gsys.pLootSystem->GetShower().Show(m_pOwner, pLootingNPC->GetUID())) return false;		
	}

	return true;
}

void GPlayerLoot::BeginByKillNPC( MUID uidNPC )
{
	m_LootingNPCUID = uidNPC;
	m_pOwner->GetDoing().ChangeDoing(CD_LOOTING);
}

void GPlayerLoot::BeginByInteractNPC(MUID uidNPC, float fTime)
{
	m_LootingNPCUID = uidNPC;
	m_pOwner->GetDoing().ChangeDoing(CD_LOOTING);

	m_pILootRegulator->SetTickTime(fTime);
	m_pILootRegulator->Start();
}

void GPlayerLoot::BeginByGatherNPC(MUID uidNPC, float fTime, int nLootID)
{
	m_LootingNPCUID = uidNPC;
	m_pOwner->GetDoing().ChangeDoing(CD_LOOTING);

	m_nLootID = nLootID;

	m_pILootRegulator->SetTickTime(fTime);
	m_pILootRegulator->Start();
}

void GPlayerLoot::EndLoot()
{
	m_LootingNPCUID = MUID::ZERO;
	m_nLootID = INVALID_ID;
	m_pILootRegulator->Stop();

	if (m_pOwner->GetDoing().IsNowLooting())
		m_pOwner->GetDoing().ChangeDoing(CD_NORMAL);
}

GEntityNPC* GPlayerLoot::GetLootingNPC()
{
	return m_pOwner->FindNPC(m_LootingNPCUID);
}

bool GPlayerLoot::IsLootingNPC(const MUID& uidNPC)
{
	return m_LootingNPCUID == uidNPC;
}

bool GPlayerLoot::IsILootRegulatorActive()
{
	return m_pILootRegulator->IsActive();
}