#include "stdafx.h"
#include "GSharedField.h"
#include "GAutoPartyMgr.h"
#include "GConfig.h"
#include "GConst.h"


GSharedField* ToSharedField( GField* pField )
{
	VALID_RET(pField, NULL);
	if (!pField->IsSharedField())			return NULL;
	return static_cast<GSharedField*>(pField);
}

CHANNELID GetChannelID( GField* pField )
{
	CHANNELID nChannelID = INVALID_CHANNELID;
	if (pField && pField->IsSharedField())
		nChannelID = ToSharedField(pField)->GetChannelID();
	return nChannelID;
}


//////////////////////////////////////////////////////////////////////////
// 
// GSharedField
//
//////////////////////////////////////////////////////////////////////////

GSharedField::GSharedField( const MUID& uid, GWeatherMgr* pWeatherMgr )
: GField(uid, pWeatherMgr)
, m_nChannelID(INVALID_CHANNELID)
, m_pAutoPartyMgr(NULL)
{
	m_pAutoPartyMgr = new GAutoPartyMgr(this);
}
GSharedField::~GSharedField()
{
	SAFE_DELETE(m_pAutoPartyMgr);
}

void GSharedField::RemoveEntity( GEntity* pEntity )
{
	__super::RemoveEntity(pEntity);

	GetAutoPartyMgr()->EraseSingle(pEntity->GetUID());

	if (pEntity->IsPlayer() &&
		GConfig::m_bStandAlone)
	{
		DestroySelf();
	}
}

void GSharedField::OnEntityLeaved( GEntity* pEntity )
{
	__super::OnEntityLeaved(pEntity);

	GetAutoPartyMgr()->EraseSingle(pEntity->GetUID());
}


bool GSharedField::IsInvalid() const
{
	return (GetChannelID() == INVALID_CHANNELID);
}

bool GSharedField::IsDefaultChannel() const
{
	VALID_RET(GetInfo(), false);
	if (GetChannelID() <= GetInfo()->m_nDefaultChannelCount)	
		return true;	// 상설 채널

	if (GetChannelID() == PRIME_CHANNELID)	
		return true;	// 첫번째 채널

	return false;
}

void GSharedField::DestroySelf()
{
	if (IsDynamicField())			return;	// 다이나믹 필드면 무시
	if (IsDefaultChannel())			return; // 상설채널이면 무시
	if (IsInvalid())				return; // 무효한 채널 아이디면 무시
	if (ExistPlayer())				return; // 플레이어가 존재하고 있다면 무시
	if (HasReservedEnterPlayers())	return;	// 필드이동 예약중인 플레이어가 있다면 무시

	DeleteMe();
}

bool GSharedField::IsEnterable()
{
	if (!__super::IsEnterable())
		return false;
	
	if (GetPlayerQty() >= GetCapacity())
		return false;

	return true;
}

int GSharedField::GetCapacity()
{
	if (m_pInfo) return m_pInfo->m_nCapacity;
	return CHANNEL_PLAYER_LIMIT;
}

void GSharedField::ReserveChangeChannel_ForStandAlone( MUID uidPlayer )
{
	m_setReservedEnterPlayers.insert(uidPlayer);
}

void GSharedField::CancelChangeChannel_ForStandAlone( MUID uidPlayer )
{
	m_setReservedEnterPlayers.erase(uidPlayer);
}

bool GSharedField::HasReservation_ForStandAlone( MUID uidPlayer )
{
	return m_setReservedEnterPlayers.find(uidPlayer) != m_setReservedEnterPlayers.end();
}

void GSharedField::Update( float fDelta )
{
	__super::Update(fDelta);

	GetAutoPartyMgr()->Update(fDelta);
}

