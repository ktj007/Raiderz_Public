#include "StdAfx.h"
#include "GDynamicFieldPolicy_InnRoom.h"
#include "GDynamicFieldMaster.h"
#include "GDynamicField.h"
#include "GInnRoomInfoMgr.h"
#include "GInnRoomInfo.h"
#include "GEntityPlayer.h"
#include "GInnSystem.h"
#include "GPlayerObjectManager.h"
#include "GGlobal.h"
#include "GSystem.h"
#include "GConst.h"


GDynamicFieldPolicy_InnRoom::GDynamicFieldPolicy_InnRoom(GDynamicFieldMaster* pOwner)
: GDynamicFieldPolicy(pOwner)
, m_expiredRegulator(GConst::DYNAMICFIELD_EXPIRED_TIME * 1000.0f)
{
}

GDynamicFieldPolicy_InnRoom::~GDynamicFieldPolicy_InnRoom()
{
}

void GDynamicFieldPolicy_InnRoom::OnCreate()
{
	GEntityPlayer* pCreator = gmgr.pPlayerObjectManager->GetEntity(m_pOwner->m_uidCreator);
	if (NULL == pCreator) return;

	GInnRoomInfo* pInnRoomInfo = gmgr.pInnRoomInfoMgr->Get(pCreator->GetPlayerInfo()->nInnRoomID);
	if (NULL == pInnRoomInfo) return;
	GDynamicField* pStartField = m_pOwner->Get(m_pOwner->GetInfo()->pStartFieldInfo->m_nFieldID);
	if (NULL == pStartField) return;

	gsys.pInnsystem->SpawnInnRoomNPC(pCreator, pInnRoomInfo, pStartField);

	m_expiredRegulator.Start();
}

void GDynamicFieldPolicy_InnRoom::OnDestroy()
{
	m_expiredRegulator.Stop();
}

void GDynamicFieldPolicy_InnRoom::OnUpdate(float fDelta)
{
	// 누군가 있다면, 유지
	if (m_pOwner->HasPreservedEnterPlayers() || !m_pOwner->m_setEntryPlayer.empty())
	{
		m_expiredRegulator.Reset();
		return;
	}
	
	// 아무도 없으면, 일정 시간 후 파괴
	if (m_expiredRegulator.IsReady(fDelta))
	{
		m_pOwner->RunSelfDestructor();
		m_expiredRegulator.Stop();
	}
}
