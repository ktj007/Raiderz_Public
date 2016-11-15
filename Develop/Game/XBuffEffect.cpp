#include "stdafx.h"
#include "XBuffEffect.h"
#include "XEffectManager.h"
#include "XBuffInfo.h"
#include "XCharacter.h"

//////////////////////////////////////////////////////////////////////////
#define BUFF_EFFECT_DISAPPEAR_TIME 3000.0f

//////////////////////////////////////////////////////////////////////////
XBuffEffect::XBuffEffect( XObject* pOwner )
: m_pObject(pOwner)
{
	m_vBuffLoopEffect.clear();
	m_vBuffDisappearEffect.clear();
	m_vEnchantBuffEffect.clear();
}

XBuffEffect::~XBuffEffect()
{
	AllDelBuffEffect();
}

bool XBuffEffect::AddBuffEffect( BUFF_EFFECT_SET_DATA& data )
{
	// 버프 정보 가져오기
	XBuffInfo * pBuffInfo = info.buff->Get(data.nBuffID);
	if(pBuffInfo == NULL)
		return false;

	// 버프 이펙트 정보 가져오기
	vector<XBuffEffectInfo *> vecEffectInfoByType;
	pBuffInfo->m_BuffEffect.GetEffectInfoList(data.nType, vecEffectInfoByType);

	// 오브젝트 NULL 검사
	// 버프이펙트가 없으면 실패
	if(m_pObject == NULL || vecEffectInfoByType.size() <= 0)
	{
		return false;
	}

	RActor* pActor = m_pObject->GetActor();
	if(pActor == NULL)
	{
		return false;
	}

	// Gain 버프
	if(data.nType == BUFF_GAIN_EFFECT)
	{
		stBUFF_EFFECT_GAIN buffGain;
		buffGain.effectData = data;
		buffGain.eState	= BEGS_START;
		m_vecGainBuffEffect.push_back(buffGain);
	}

	// 버프 이펙트 루프
	vector<XBuffEffectInfo *>::iterator itBuffEffect = vecEffectInfoByType.begin();
	while(itBuffEffect != vecEffectInfoByType.end())
	{
		vector<EFFECT_RESULT_DATA> vecResult;

		if((*itBuffEffect)->m_bBuffWeaponEffect)
		{
			int nPartsSlotType = data.nPartsSlotType;
			if(nPartsSlotType == -1)
				nPartsSlotType = (int)PLAYER_PARTS_SLOT_RWEAPON;

			XEffectWeaponInvoker effectInvoker;
			effectInvoker.Invoke(m_pObject, (*itBuffEffect)->m_strEffectName, (*itBuffEffect)->m_strBoneName, nPartsSlotType, &vecResult);
		}
		else
		{
			XEffectInvoker effectInvoker;
			effectInvoker.Invoke(m_pObject, (*itBuffEffect)->m_strEffectName, wstring(), (*itBuffEffect)->m_strBoneName, &vecResult);
		}

		if(data.nType == BUFF_GAIN_EFFECT)
		{
			stBUFF_EFFECT_GAIN* pGainEffect = GetGainBuffEffect(data.nBuffID);
			if(pGainEffect)
			{
				SetGainBuffEffect(vecResult, data.nBuffID, pGainEffect->vecUIDStart);
			}
		}

		SetBuffLoopEffect(vecResult, data.nBuffID, data.bRemoveOncePlay);		

		itBuffEffect++;
	}

	return true;
}

void XBuffEffect::DelBuffEffect( int nBuffID, int nPartsSlotType )
{
	// 루프 버프 이펙트 처리
	vector<BUFF_EFFECT_LOOP>::iterator itBuff = m_vBuffLoopEffect.begin();
	while(itBuff != m_vBuffLoopEffect.end())
	{
		if((*itBuff).nBuffID == nBuffID)
		{
			if((*itBuff).type == CET_MODEL)
			{
				global.emgr->DeleteXEffect((*itBuff).uidEffect);
			}
			else if((*itBuff).type == CET_SOUND)
			{
				if ( global.sound)
					global.sound->StopDynamic3D( (*itBuff).uidEffect);
			}

			itBuff = m_vBuffLoopEffect.erase(itBuff);
			continue;
		}

		++itBuff;
	}


	// Gain 버프 처리
	stBUFF_EFFECT_GAIN* pBuffEffect = GetGainBuffEffect(nBuffID);
	if(pBuffEffect)
	{
		DelLoopBuffEffect(pBuffEffect->vecUIDStart);
		DelLoopBuffEffect(pBuffEffect->vecUIDLoop);

		pBuffEffect->vecUIDStart.clear();
		pBuffEffect->vecUIDLoop.clear();

		// relase effect
		CreateBuffEffect(nBuffID, BUFF_GAIN_RELEASE_EFFECT);

		DelGainBuffEffect(nBuffID);
	}

	// 강화 버프 처리
	vector<stEnchantBuffEffect>::iterator itEnchantBuff = m_vEnchantBuffEffect.begin();
	while(itEnchantBuff != m_vEnchantBuffEffect.end())
	{
		if(itEnchantBuff->nBuffID == nBuffID && 
			itEnchantBuff->nPartsSlotType == nPartsSlotType)
		{
			DelLoopBuffEffect(itEnchantBuff->vecEnchantBuffEffect, true);
			itEnchantBuff = m_vEnchantBuffEffect.erase(itEnchantBuff);
			continue;
		}

		++itEnchantBuff;
	}
}

void XBuffEffect::AllDelBuffEffect( bool bImmediatlyDelete /*= false*/ )
{
	DelLoopBuffEffect(m_vBuffLoopEffect, bImmediatlyDelete);
	m_vBuffLoopEffect.clear();

	for(vector<stEnchantBuffEffect>::iterator itEnchantBuff = m_vEnchantBuffEffect.begin(); itEnchantBuff != m_vEnchantBuffEffect.end(); ++itEnchantBuff)
		DelLoopBuffEffect(itEnchantBuff->vecEnchantBuffEffect, bImmediatlyDelete);
	m_vEnchantBuffEffect.clear();

	for(vector<stBUFF_EFFECT_GAIN>::iterator it = m_vecGainBuffEffect.begin(); it != m_vecGainBuffEffect.end(); ++it)
	{
		DelLoopBuffEffect(it->vecUIDStart, bImmediatlyDelete);
		DelLoopBuffEffect(it->vecUIDLoop, bImmediatlyDelete);

		it->vecUIDStart.clear();
		it->vecUIDLoop.clear();
	}

	m_vecGainBuffEffect.clear();
}

void XBuffEffect::DieBuffEffect( int nBuffID )
{
	vector<BUFF_EFFECT_LOOP>::iterator itBuff = m_vBuffLoopEffect.begin();
	while(itBuff != m_vBuffLoopEffect.end())
	{
		if((*itBuff).nBuffID == nBuffID)
		{
			if((*itBuff).type == CET_MODEL)
			{
				BUFF_EFFECT_DISAPPEAR disappearEffect;
				disappearEffect.buffEffect = (*itBuff);
				disappearEffect.fDisappearTime = 0.0f;
					
				m_vBuffDisappearEffect.push_back(disappearEffect);
			}

			itBuff = m_vBuffLoopEffect.erase(itBuff);
			continue;
		}

		itBuff++;
	}
}

void XBuffEffect::Update( float fDelta )
{
	PFC_THISFUNC;
	UpdateDisAppearFadeHide(fDelta);
	UpdateGainEffect();
}

void XBuffEffect::UpdateDisAppearFadeHide( float fDelta )
{
	vector<BUFF_EFFECT_DISAPPEAR>::iterator itDisappear = m_vBuffDisappearEffect.begin();
	while(itDisappear != m_vBuffDisappearEffect.end())
	{
		if((*itDisappear).fDisappearTime > 0.0f)
		{
			(*itDisappear).fDisappearTime -= (fDelta * 1000.0f);
			float fVisibility = (*itDisappear).fDisappearTime / BUFF_EFFECT_DISAPPEAR_TIME;

			if(fVisibility > 0.0f)
			{
				XEffect* pEffect = global.emgr->GetXEffect((*itDisappear).buffEffect.uidEffect);
				if(pEffect)
				{
					pEffect->GetEffect()->SetVisibility(fVisibility);
				}
			}
		}

		if((*itDisappear).fDisappearTime <= 0.0f)
		{
			(*itDisappear).fDisappearTime = 0.0f;

			// 끝났으면... 삭제
			global.emgr->DeleteXEffect((*itDisappear).buffEffect.uidEffect);

			itDisappear = m_vBuffDisappearEffect.erase(itDisappear);
			continue;
		}

		itDisappear++;
	}
}

void XBuffEffect::DelLoopBuffEffect( VEC_BUFF_EFFECT_LOOP& vecEffect, bool bImmediatlyDelete /*= false*/ )
{
	VEC_BUFF_EFFECT_LOOP::iterator itBuff = vecEffect.begin();
	while(itBuff != vecEffect.end())
	{
		if((*itBuff).type == CET_MODEL)
		{
			global.emgr->DeleteXEffect((*itBuff).uidEffect, bImmediatlyDelete);
		}
		else if((*itBuff).type == CET_SOUND)
		{
			if ( global.sound)
				global.sound->StopSound( (*itBuff).uidEffect);
		}

		itBuff++;
	}
}

void XBuffEffect::UpdateGainEffect()
{
	for(vector<stBUFF_EFFECT_GAIN>::iterator it = m_vecGainBuffEffect.begin(); it != m_vecGainBuffEffect.end(); ++it)
	{
		if(it->eState == BEGS_START)
		{
			if(CheckExistEffect(it->vecUIDStart) == false)
			{
				// loop로
				vector<EFFECT_RESULT_DATA> vecResult;
				CreateBuffEffect(it->effectData.nBuffID, BUFF_GAIN_LOOP_EFFECT, &vecResult);
				SetGainBuffEffect(vecResult, it->effectData.nBuffID, it->vecUIDLoop);
				it->eState = BEGS_LOOP;
			}
		}
	}
}

void XBuffEffect::SetBuffLoopEffect( vector<EFFECT_RESULT_DATA>& vecResult, int nBuffID, bool bRemoveOncePlay )
{
	for(vector<EFFECT_RESULT_DATA>::iterator itUID = vecResult.begin(); itUID != vecResult.end(); itUID++)
	{
		XModelEffect* pEffect = global.emgr->GetXModelEffect((*itUID).uidEffect);
		if(pEffect)
		{
			if((pEffect->IsEffectLoop() == false))
			{
				if(bRemoveOncePlay)
				{
					// 한번 플레이가 되는 녀석을 제거하란다. 버프이펙트만이 가진 특별 상항이다.
					global.emgr->DeleteXEffect((*itUID).uidEffect, true);
				}

				continue;
			}

			BUFF_EFFECT_LOOP buff_effect((*itUID).type, nBuffID, (*itUID).uidEffect);
			m_vBuffLoopEffect.push_back(buff_effect);
		}
	}
}

void XBuffEffect::SetGainBuffEffect( vector<EFFECT_RESULT_DATA>& vecResult, int nBuffID, VEC_BUFF_EFFECT_LOOP& vecBuffEffect )
{
	for(vector<EFFECT_RESULT_DATA>::iterator itUID = vecResult.begin(); itUID != vecResult.end(); itUID++)
	{
		if(itUID->type == CET_MODEL)
		{
			XEffect* pEffect = global.emgr->GetXEffect((*itUID).uidEffect);
			if(pEffect)
			{
				BUFF_EFFECT_LOOP buff_effect((*itUID).type, nBuffID, (*itUID).uidEffect);
				vecBuffEffect.push_back(buff_effect);
			}
		}
		else if(itUID->type == CET_SOUND)
		{
			BUFF_EFFECT_LOOP buff_effect((*itUID).type, nBuffID, (*itUID).uidEffect);
			vecBuffEffect.push_back(buff_effect);
		}
	}
}

bool XBuffEffect::CheckExistEffect( VEC_BUFF_EFFECT_LOOP& vecEffect )
{
	for(VEC_BUFF_EFFECT_LOOP::iterator itBuff = vecEffect.begin(); itBuff != vecEffect.end(); ++itBuff)
	{
		if(global.emgr->GetXEffect((*itBuff).uidEffect))
			return true;
	}

	return false;
}

void XBuffEffect::CreateBuffEffect( int nID, BUFF_EFFECT_TYPE nType, vector<EFFECT_RESULT_DATA>* pvecResult /*= NULL*/ )
{
	// 버프 정보 가져오기
	XBuffInfo * pBuffInfo = info.buff->Get(nID);
	if(pBuffInfo == NULL)
		return;

	// 버프 이펙트 정보 가져오기
	vector<XBuffEffectInfo *> vecEffectInfoByType;
	pBuffInfo->m_BuffEffect.GetEffectInfoList(nType, vecEffectInfoByType);

	// 버프 이펙트
	for(vector<XBuffEffectInfo *>::iterator itBuffEffect = vecEffectInfoByType.begin(); itBuffEffect != vecEffectInfoByType.end(); ++itBuffEffect)
	{
		vector<EFFECT_RESULT_DATA> vecResult;

		if((*itBuffEffect)->m_bBuffWeaponEffect)
		{
			XEffectWeaponInvoker effectInvoker;
			effectInvoker.Invoke(m_pObject, (*itBuffEffect)->m_strEffectName, wstring(), 0, pvecResult);
		}
		else
		{
			XEffectInvoker effectInvoker;
			effectInvoker.Invoke(m_pObject, (*itBuffEffect)->m_strEffectName, wstring(), (*itBuffEffect)->m_strBoneName, pvecResult);
		}
	}
}

stBUFF_EFFECT_GAIN* XBuffEffect::GetGainBuffEffect( int nID )
{
	for(vector<stBUFF_EFFECT_GAIN>::iterator it = m_vecGainBuffEffect.begin(); it != m_vecGainBuffEffect.end(); ++it)
	{
		if(it->effectData.nBuffID == nID)
			return &(*it);
	}

	return NULL;
}

void XBuffEffect::DelGainBuffEffect( int nID )
{
	for(vector<stBUFF_EFFECT_GAIN>::iterator it = m_vecGainBuffEffect.begin(); it != m_vecGainBuffEffect.end(); ++it)
	{
		if(it->effectData.nBuffID == nID)
		{
			m_vecGainBuffEffect.erase(it);
			return;
		}
	}
}

void XBuffEffect::Init_ForChangeField()
{
	vector<stBUFF_EFFECT_GAIN>		vecGainBuffEffect;
	vector<stEnchantBuffEffect>		vecEnchantBuffEffect;

	for(vector<stBUFF_EFFECT_GAIN>::iterator it = m_vecGainBuffEffect.begin(); it != m_vecGainBuffEffect.end(); ++it)
	{
		vecGainBuffEffect.push_back(*it);
	}

	for(vector<stEnchantBuffEffect>::iterator itEnchantTemp = m_vEnchantBuffEffect.begin(); itEnchantTemp != m_vEnchantBuffEffect.end(); ++itEnchantTemp)
	{
		vecEnchantBuffEffect.push_back(*itEnchantTemp);
	}

	AllDelBuffEffect(true);

	for(vector<stBUFF_EFFECT_GAIN>::iterator itTemp = vecGainBuffEffect.begin(); itTemp != vecGainBuffEffect.end(); ++itTemp)
	{
		if(itTemp->eState == BEGS_START)
		{
			AddBuffEffect(itTemp->effectData);
		}
		else if(itTemp->eState == BEGS_LOOP)
		{
			stBUFF_EFFECT_GAIN buffGain;
			buffGain.effectData = itTemp->effectData;
			buffGain.eState	= BEGS_LOOP;
			m_vecGainBuffEffect.push_back(buffGain);

			stBUFF_EFFECT_GAIN* pEffectGain = GetGainBuffEffect(itTemp->effectData.nBuffID);

			vector<EFFECT_RESULT_DATA> vecResult;
			CreateBuffEffect(itTemp->effectData.nBuffID, BUFF_GAIN_LOOP_EFFECT, &vecResult);
			SetGainBuffEffect(vecResult, itTemp->effectData.nBuffID, pEffectGain->vecUIDLoop);
		}
	}

	for(vector<stEnchantBuffEffect>::iterator itEnchant = vecEnchantBuffEffect.begin(); itEnchant != vecEnchantBuffEffect.end(); ++itEnchant)
	{
		CreateEnchantBuffEffect(itEnchant->nBuffID, itEnchant->strResourceName, itEnchant->strBoneName, itEnchant->nPartsSlotType);
	}
}

bool XBuffEffect::AddEnchantBuffEffect( int nBuffID, int nPartsSlotType )
{
	// 버프 정보 가져오기
	XBuffInfo * pBuffInfo = info.buff->Get(nBuffID);
	if(pBuffInfo == NULL)
		return false;

	// 강화 버프 이펙트 정보 가져오기
	XActor* pXActor = m_pObject->AsActor();
	WEAPON_TYPE eRType = pXActor->GetEquipWeaponType(false);
	WEAPON_TYPE eLType = pXActor->GetEquipWeaponType(true);

	vector<XEnchantBuffEffectInfo *> vecEnchantEffectInfoByType;
	pBuffInfo->m_EnchantBuffEffect.GetEffectInfoList(EBET_DUMMY_EFFECT, eRType, eLType, nPartsSlotType, vecEnchantEffectInfoByType);

	// 오브젝트 NULL 검사
	// 버프이펙트가 없으면 실패
	if(m_pObject == NULL || vecEnchantEffectInfoByType.size() <= 0)
	{
		return false;
	}

	RActor* pActor = m_pObject->GetActor();
	if(pActor == NULL)
	{
		return false;
	}

	// 강화 버프 이펙트
	for(vector<XEnchantBuffEffectInfo *>::iterator itBuffEffect = vecEnchantEffectInfoByType.begin(); itBuffEffect != vecEnchantEffectInfoByType.end(); ++itBuffEffect)
	{
		CreateEnchantBuffEffect(nBuffID, (*itBuffEffect)->m_strEffectName, (*itBuffEffect)->m_strBoneName, nPartsSlotType);
	}

	return true;
}

void XBuffEffect::CreateEnchantBuffEffect( int nBuffID, wstring& strEffectName, wstring& strBoneName, int nPartsSlotType )
{
	vector<EFFECT_RESULT_DATA> vecResult;
	XEffectWeaponInvoker effectInvoker;
	effectInvoker.Invoke(m_pObject, strEffectName, strBoneName, nPartsSlotType, &vecResult);

	if(vecResult.size() > 0)
	{
		stEnchantBuffEffect enchantBuffEffect;
		enchantBuffEffect.nBuffID = nBuffID;
		enchantBuffEffect.strResourceName = strEffectName;
		enchantBuffEffect.strBoneName = strBoneName;
		enchantBuffEffect.nPartsSlotType = nPartsSlotType;

		for(vector<EFFECT_RESULT_DATA>::iterator it = vecResult.begin(); it != vecResult.end(); ++it)
		{
			BUFF_EFFECT_LOOP effectLoop;
			effectLoop.nBuffID = nBuffID;
			effectLoop.type = it->type;
			effectLoop.uidEffect = it->uidEffect;

			enchantBuffEffect.vecEnchantBuffEffect.push_back(effectLoop);
		}

		m_vEnchantBuffEffect.push_back(enchantBuffEffect);
	}
}