#include "StdAfx.h"
#include "GTalentFocus.h"
#include "GEquipmentLimiter.h"
#include "GEntityPlayer.h"
#include "GBuffInfo.h"
#include "GModuleBuff.h"
#include "GBuff.h"
#include "GGlobal.h"
#include "GItemHolder.h"

GTalentFocus::GTalentFocus( GEntityPlayer* pOnwer ) 
: m_pOwner(pOnwer)
, m_bActive(false)
, m_pEquipmentLimiter(NULL)
, m_nBuffType(INVALID_BUFF_ID)
{
	assert(m_pOwner);
	m_pOwner->AddRef();

	GItemHolder* pItemHolder = pOnwer->GetItemHolder();
	m_pEquipmentLimiter = new GEquipmentLimiter(pItemHolder);
}

GTalentFocus::~GTalentFocus()
{
	SAFE_DELETE(m_pEquipmentLimiter);

	m_pOwner->Drop();
}

void GTalentFocus::AddEnableWeapon( WEAPON_TYPE nWeaponType )
{
	VALID(m_pEquipmentLimiter);
	m_pEquipmentLimiter->AddEnableWeapon(nWeaponType);
}

bool GTalentFocus::CheckEquipItem()
{
	VALID_RET(m_pEquipmentLimiter, false);
	return m_pEquipmentLimiter->IsEnable();
}

void GTalentFocus::Active()
{
	m_bActive = true;
	GainFocusBuff();
}

void GTalentFocus::Deactive()
{
	m_bActive = false;
	LostFocusBuff();
}

void GTalentFocus::GainFocusBuff()
{
	GModuleBuff* pmBuff = m_pOwner->GetModuleBuff();
	VALID(pmBuff);

	GBuffInfo* pBuffInfo = gmgr.pBuffInfoMgr->Get(GetFocusBuffID());
	VALID(pBuffInfo);
	if (!IsGainViaBuff())
	{
		// 버프로 포커스를 얻지 않은 경우에는 버프를 얻음
		pmBuff->GainBuffForced(pBuffInfo, GetDurationTime(), 0.0f);
	}
}

void GTalentFocus::LostFocusBuff()
{
	GModuleBuff* pmBuff = m_pOwner->GetModuleBuff();
	VALID(pmBuff);

	pmBuff->CancelBuff(GetFocusBuffID());
}

int GTalentFocus::GetFocusBuffID()
{
	TALENT_FOCUS_TYPE nFocusType =	GetType();
	if (nFocusType < 0)
		return INVALID_BUFF_ID;

	if (nFocusType >= TFT_SIZE)
		return INVALID_BUFF_ID;

	int nBuffID = BUFF_FOCUS_ID[nFocusType];
	if (m_nBuffType != INVALID_BUFF_ID)
	{
		nBuffID += m_nBuffType - 1;	// 1은 ExtraActiveValue1의 기본값이 1이기 때문에 편의상 빼줌
	}
	return nBuffID;
}

void GTalentFocus::SetBuffType( int nBuffType )
{
	m_nBuffType = nBuffType;
}