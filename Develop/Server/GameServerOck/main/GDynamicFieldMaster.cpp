#include "StdAfx.h"
#include "GDynamicFieldMaster.h"
#include "GDynamicField.h"
#include "GConst.h"
#include "GEntityPlayer.h"
#include "GDynamicFieldPolicy.h"
#include "GPlayerObjectManager.h"
#include "GGlobal.h"
#include "GSTLUtil.h"
#include "GDynamicFieldPolicy_TrialField.h"
#include "GDef.h"

//////////////////////////////////////////////////////////////////////////
//
//	GDynamicFieldMaster
//
//////////////////////////////////////////////////////////////////////////

GDynamicFieldMaster::GDynamicFieldMaster(const MUID& uidCreator, const DYNAMIC_FIELD_TYPE eType)
: m_pInfo(NULL)
, m_eType(eType)
, m_bDeleteMe(false)
, m_pSelfDestructor(NULL)
, m_pPolicy(NULL)
, m_uidCreator(uidCreator)
{
	m_pPolicy = new GDynamicFieldPolicyMLeaf(this);
}

GDynamicFieldMaster::~GDynamicFieldMaster()
{
	SAFE_DELETE(m_pSelfDestructor);
	SAFE_DELETE(m_pPolicy);
}

void GDynamicFieldMaster::Create( MUID uidNew, FIELD_GROUP_INFO* pInfo )
{
	m_UID = uidNew;
	m_pInfo = pInfo;

	m_pPolicy->Create();
}

void GDynamicFieldMaster::Destroy()
{
	
}

void GDynamicFieldMaster::DeleteMe()
{
	m_bDeleteMe = true;

	for each (const MAP_DYNAMICFIELD::value_type& data in m_mapFieldByID)
	{
		data.second->DeleteMe();
	}
}

void GDynamicFieldMaster::Insert(GDynamicField* pField)
{
	if (!pField) return;

	m_mapFieldByID.insert(MAP_DYNAMICFIELD::value_type(pField->GetInfo()->m_nFieldID, pField));
}

GDynamicField* GDynamicFieldMaster::Get(int nFieldID)
{
	MAP_DYNAMICFIELD::iterator itor = m_mapFieldByID.find(nFieldID);
	if (m_mapFieldByID.end() == itor) return NULL;

	return (*itor).second;
}

void GDynamicFieldMaster::Update(float fDelta)
{
	if (m_pSelfDestructor)
	{
		m_pSelfDestructor->Update();
		return;
	}

	m_pPolicy->Update(fDelta);
}


bool GDynamicFieldMaster::HasPreservedEnterPlayers()
{
	for each (const MAP_DYNAMICFIELD::value_type& data in m_mapFieldByID)
	{
		if (data.second->HasReservedEnterPlayers())
			return true;
	}

	return false;
}

bool GDynamicFieldMaster::KickAllPlayers()
{
	while (m_setEntryPlayer.begin() != m_setEntryPlayer.end())
	{
		set<MUID>::iterator itor = m_setEntryPlayer.begin();
		MUID uidPlayer = (*itor);

		GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(uidPlayer);
		if (NULL == pPlayer)
		{
			m_setEntryPlayer.erase(itor);
			continue;
		}

		OnLeavePlayer(pPlayer);
	}	

	for each (const MAP_DYNAMICFIELD::value_type& data in m_mapFieldByID)
	{
		if (!data.second->KickAllPlayers())
			return false;
	}

	return true;
}

bool GDynamicFieldMaster::OnEnterPlayer( GEntityPlayer* pPlayer )
{
	if (NULL == pPlayer)						return false;
	if (IsEnteredPlayer(pPlayer->GetUID()))		return false;

	InsertPlayer(pPlayer);

	return m_pPolicy->EnterPlayer(pPlayer);	
}

void GDynamicFieldMaster::OnLeavePlayer( GEntityPlayer* pPlayer )
{
	assert(pPlayer);
	if (!pPlayer)		return;

	ErasePlayer(pPlayer);

	m_pPolicy->LeavePlayer(pPlayer);
}

void GDynamicFieldMaster::InsertPlayer(GEntityPlayer* pPlayer)
{
	if (NULL == pPlayer) return;
	
	if (DPT_TRIAL == m_pPolicy->GetType())
	{
		pPlayer->AttachFieldObserver(m_UID);
	}
	m_setEntryPlayer.insert(pPlayer->GetUID());
}

void GDynamicFieldMaster::ErasePlayer(GEntityPlayer* pPlayer)
{
	if (NULL == pPlayer) return;

	if (DPT_TRIAL == m_pPolicy->GetType())
	{
		pPlayer->DetachFieldObserver(m_UID);
	}

	m_setEntryPlayer.erase(pPlayer->GetUID());
}

bool GDynamicFieldMaster::IsEnteredPlayer( const MUID& uidPlayer )
{
	return m_setEntryPlayer.find(uidPlayer) != m_setEntryPlayer.end();
}

bool GDynamicFieldMaster::IsPlayerExistInField(MUID uidField)
{
	for each (const MAP_DYNAMICFIELD::value_type& data in m_mapFieldByID)
	{
		if (data.second->GetUID() == uidField)
			return true;
	}

	return false;
}

void GDynamicFieldMaster::RunSelfDestructor()
{
	if (m_pSelfDestructor)
		return;

	m_pSelfDestructor = new SelfDestructor(this);
}

void GDynamicFieldMaster::SetPolicy( GDynamicFieldPolicy* pPolicy )
{
	if (NULL == pPolicy) return;

	SAFE_DELETE(m_pPolicy);
	m_pPolicy = pPolicy;
}

int GDynamicFieldMaster::GetPlayerQty()
{
	int nQty = 0;

	for each (const MAP_DYNAMICFIELD::value_type& data in m_mapFieldByID)
	{
		nQty += data.second->GetPlayerQty();
	}

	return nQty;
}

FIELD_ENTER_FAIL_TYPE GDynamicFieldMaster::IsEnterable()
{
	VALID_RET(GetPolicy(), ENTERFAIL_SYSTEM_ERROR);

	return GetPolicy()->IsEnterable();
}










//////////////////////////////////////////////////////////////////////////
//
//	GDynamicFieldMaster::SelfDestructor
//
//////////////////////////////////////////////////////////////////////////

GDynamicFieldMaster::SelfDestructor::SelfDestructor( GDynamicFieldMaster* pOwner )
: m_pOwner(pOwner)
, m_bReadyToDestroy(false)
{
	assert(m_pOwner);
}

void GDynamicFieldMaster::SelfDestructor::Update()
{
	if (m_pOwner->HasPreservedEnterPlayers())
	{
		return;
	}

	if (false == m_bReadyToDestroy)
	{
		ReadyToDestroy();
	}

	if (0 == m_pOwner->GetPlayerQty())
	{
		Destory();
	}
}

void GDynamicFieldMaster::SelfDestructor::ReadyToDestroy()
{
	m_bReadyToDestroy = true;

	if (!m_pOwner->KickAllPlayers())	// 엔트리 필드에 참가한 모든 플레이어를 쫓아낸다.
	{
		// Kick failed!!
		dlog("GDynamicFieldGroup::OnExpired(), KickAllPlayers() is failed");
		assert(FALSE && L"GDynamicFieldGroup::OnExpired(), KickAllPlayers() is failed");
		return;
	}
}

void GDynamicFieldMaster::SelfDestructor::Destory()
{
	m_pOwner->DeleteMe();
	m_pOwner->m_pPolicy->Destroy();
}

bool GDynamicFieldMaster::IsNowCombat() const
{
	for each (const MUID& uidPlayer in m_setEntryPlayer)
	{
		GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(uidPlayer);
		if (!pPlayer) 
			continue;

		if (pPlayer->IsNowCombat())
			return true;
	}	

	return false;
}

bool GDynamicFieldMaster::IsChallengerQuestField()
{
	VALID_RET(GetInfo(), false);

	return GetInfo()->bChallengerQuest;
}