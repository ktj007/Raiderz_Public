#include "stdafx.h"
#include "XTalentEffect.h"
#include "XTalentEffectMgr.h"
#include "XTalentInfoMgr.h"
#include "XCharacter.h"
#include "XNonPlayer.h"
#include "XModuleCollision.h"
#include "XModuleMovable.h"
#include "XGame.h"

// 디버그용
//#include "RDebugPassRenderer.h"

//////////////////////////////////////////////////////////////////////////
#define DEFAULT_EFFECT_FILE_NAME	L"meleeattack01"
#define BLOOD_EFFECT_COUNT			2

//////////////////////////////////////////////////////////////////////////

XTalentEffect::XTalentEffect()
{
	m_vecDelEffectList.clear();
	m_vecDelSoundList.clear();

	m_mapTalentEffectHandlers.insert( map<TALENT_EFFECT_TYPE, TALENTEFFECTHANDLER>::value_type( CAST_EFFECT,		CreateCastEffect) );
	m_mapTalentEffectHandlers.insert( map<TALENT_EFFECT_TYPE, TALENTEFFECTHANDLER>::value_type( USE_EFFECT,			CreateUseEffect) );
	m_mapTalentEffectHandlers.insert( map<TALENT_EFFECT_TYPE, TALENTEFFECTHANDLER>::value_type( ATTACK_EFFECT,		CreateAttackEffect) );
	m_mapTalentEffectHandlers.insert( map<TALENT_EFFECT_TYPE, TALENTEFFECTHANDLER>::value_type( ATTACK_SECONDARY_EFFECT,		CreateAttackEffect) );
	m_mapTalentEffectHandlers.insert( map<TALENT_EFFECT_TYPE, TALENTEFFECTHANDLER>::value_type( MISS_EFFECT,		CreateMissEffect) );
	m_mapTalentEffectHandlers.insert( map<TALENT_EFFECT_TYPE, TALENTEFFECTHANDLER>::value_type( SHOCK_EFFECT,		CreateShockEffect) );
	m_mapTalentEffectHandlers.insert( map<TALENT_EFFECT_TYPE, TALENTEFFECTHANDLER>::value_type( DURATION_EFFECT,	CreateDurationEffect) );
	m_mapTalentEffectHandlers.insert( map<TALENT_EFFECT_TYPE, TALENTEFFECTHANDLER>::value_type( DEFENSE_EFFECT,		CreateDefenseEffect) );
	m_mapTalentEffectHandlers.insert( map<TALENT_EFFECT_TYPE, TALENTEFFECTHANDLER>::value_type( CAST_END_EFFECT,	CreateCastEndEffect) );

	m_bHaveAttackSound = false;
	m_bCheckAttackSound = false;
}

XTalentEffect::~XTalentEffect()
{
	ClearEffect();
}

void XTalentEffect::CreateByTalentEffectType( CREATE_TALENT_EFFECT_DATA& creatTalentEffectData )
{
	if (creatTalentEffectData.pTalentInfo == NULL || creatTalentEffectData.pOwner == NULL) 
		return;

	//////////////////////////////////////////////////////////////////////////
	// 준비
	XTalentEffectDataMgr* pTalentEffectDataMgr = creatTalentEffectData.pTalentInfo->GetEffectDataMgr();

	// nType으로 이펙트Info를 가져온다.
	vector<XTalentEffectInfo *> vecEffectInfoByType;
	if(pTalentEffectDataMgr && pTalentEffectDataMgr->IsEmpty() == false)
	{
		pTalentEffectDataMgr->Get(creatTalentEffectData.nType, vecEffectInfoByType);
	}

	m_bHaveAttackSound = false;
	m_bCheckAttackSound = false;
	//////////////////////////////////////////////////////////////////////////
	// 이펙트 발생
	bool bModelEffect = false;
	vec3 vDir = vec3::ZERO;
	vector<XTalentEffectInfo*>::iterator it = vecEffectInfoByType.begin();
	while(it != vecEffectInfoByType.end())
	{
		vDir = GetTalentEffectDir((*it), creatTalentEffectData.pOwner, creatTalentEffectData.pAttacker);

		// 판정 캡슐이라면 걸러낸다.
		if((*it)->m_nImpactPos == TALENT_HIT_CAPSULE_POS)
		{
			vec3 vTalentDir = creatTalentEffectData.pOwner->GetDirection();
			if(creatTalentEffectData.pOwner->GetUID() != XGetMyUID())
				vTalentDir = creatTalentEffectData.pOwner->GetModuleMovable()->GetMovableFactor().INTERPOLATION.m_vTargetDir;

			talentEffectHitCapsule.AddTalentHitCapsuleImpact(creatTalentEffectData.pOwner->GetUID(), creatTalentEffectData.pTalentInfo, (*it), creatTalentEffectData.nType, -vTalentDir);
		}
		else
		{
			TALENT_EFFECT_INFO_DATA talentEffectInfoData;
			map<TALENT_EFFECT_TYPE, TALENTEFFECTHANDLER>::iterator itTalentEffect = m_mapTalentEffectHandlers.find((*it)->m_nEffectType);
			if(itTalentEffect != m_mapTalentEffectHandlers.end())
			{
				talentEffectInfoData.Set(*it, creatTalentEffectData.pOwner, creatTalentEffectData.effectPos, vDir, creatTalentEffectData.matEffect, creatTalentEffectData.pAttacker, creatTalentEffectData.nDamageFlags);
				itTalentEffect->second(&talentEffectInfoData);
			}

			if(creatTalentEffectData.nType == ATTACK_EFFECT)
			{
				m_bCheckAttackSound = true;
				CheckAttackEffectSound(talentEffectInfoData.vecOutResultData);
			}

			for(vector<EFFECT_RESULT_DATA>::iterator it = talentEffectInfoData.vecOutResultData.begin(); it != talentEffectInfoData.vecOutResultData.end(); it++)
			{
				if((*it).type == CET_MODEL)
				{
					if(creatTalentEffectData.bEffectStorage)
					{
						EFFECT_DEL EffectDel;
						EffectDel.uidEffect			= (*it).uidEffect;
						m_vecDelEffectList.push_back(EffectDel);
					}

					// 모델 이펙트 체크
					// 없으면... 디폴트 이펙트 발생해야 하낟.
					bModelEffect = true;
				}

				if((*it).type == CET_SOUND)
				{
					if(creatTalentEffectData.bEffectStorage)
					{
						m_vecDelSoundList.push_back((*it).uidEffect);
					}
				}
			}
		}

		it++;
	}

	//////////////////////////////////////////////////////////////////////////
	// 피격 사운드
	if(((creatTalentEffectData.nType == ATTACK_EFFECT || creatTalentEffectData.nType == ATTACK_SECONDARY_EFFECT) && vecEffectInfoByType.empty()) || 
		(m_bCheckAttackSound == true && m_bHaveAttackSound == false))
	{
		// 없으면 모델에서 찾자
		if(creatTalentEffectData.pAttacker && creatTalentEffectData.pAttacker->GetActor() && creatTalentEffectData.pAttacker->GetActor()->GetAnimationHitSoundInfo())
		{
			wstring strSoundName = creatTalentEffectData.pAttacker->GetActor()->GetAnimationHitSoundInfo()->Get(creatTalentEffectData.pAttacker->GetCurrentAnimationName());
			if ( strSoundName.empty() == false)
			{
				if ( global.sound)
					global.sound->PlayDynamic3D( MLocale::ConvUTF16ToAnsi(strSoundName.c_str()).c_str(), NULL, 0, creatTalentEffectData.pOwner->GetUID());
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	// 왜 피 이펙트가 별도로 표기가 되었냐면...
	// 탤런트 이펙트에... 피 이펙트 정보가 없다.
	// 이건 그냥 디폴트 형식으로 뿌려주는것이다.
	if(creatTalentEffectData.nType == BLOOD_EFFECT)
	{
		// 젠장.. 이건 좀 아니다. ㅠㅠ(XModuleEffect로 어떻게 좀 해야...)
		// 피 이펙트가 보이는 탤런트인가?
		vec3 vOtherDir = creatTalentEffectData.pOwner->GetDirection() * -1.0f;
		if(creatTalentEffectData.pAttacker != NULL)
			vOtherDir = creatTalentEffectData.pAttacker->GetDirection();

		if(creatTalentEffectData.pTalentInfo->m_bUseBloodEffect == true)
		{
			bool bCri = false;
			if (CheckBitSet(creatTalentEffectData.nDamageFlags, CTR_CRITICAL)) 
				bCri = true;

			CreateAttackBloodEffect(creatTalentEffectData.pOwner, creatTalentEffectData.effectPos, vOtherDir, bCri, creatTalentEffectData.pTalentInfo->m_fHitBloodScale, creatTalentEffectData.matEffect);					// 피이펙트
		}
	}

	// 디폴트 이펙트 발생
	if( vecEffectInfoByType.empty() || bModelEffect == false)
	{
		// 2008-12-10 디폴트 이펙트 내용 변경
		// 아무 내용 및 기술이 안돼어 있다면... 디폴트로
		CreateDefaultTalentEffect(creatTalentEffectData);
		return;
	}

	vecEffectInfoByType.clear();
}

void XTalentEffect::CreateDefaultTalentEffect( CREATE_TALENT_EFFECT_DATA& creatTalentEffectData )
{
	//테스트 코드 옮김-필요없음 지워주삼.
	if(creatTalentEffectData.pOwner)
	{
		if(creatTalentEffectData.nType == ATTACK_EFFECT ||
			creatTalentEffectData.nType == ATTACK_SECONDARY_EFFECT)
		{
			return CreateAttackDefaultTalentEffect(creatTalentEffectData);
		}
		else if(creatTalentEffectData.nType == DEFENSE_EFFECT)
		{
			return CreateDefenseDefaultTalentEffect(creatTalentEffectData.pOwner, creatTalentEffectData.effectPos, creatTalentEffectData.pAttacker);
		}
	}
	return;
}

void XTalentEffect::CreateCastEffect( TALENT_EFFECT_INFO_DATA* pInfo )
{
	EFFECT_POST_TRANSFORM postData;
	if(pInfo->pEffectInfo->m_nEffectDirection == TED_AUTO_DIR_ATTACK_EFFECT &&
		pInfo->matEffect != MMatrix::IDENTITY)
	{
		postData.matWorldMatrix = pInfo->matEffect;
	}
	else if(pInfo->effectPos != vec3::ZERO || 
			pInfo->effectDir != vec3::AXISY)
	{
		postData.matWorldMatrix.SetLocalMatrix(pInfo->effectPos, pInfo->effectDir, vec3::AXISZ);
	}

	if(pInfo->pAttacker && pInfo->pAttacker->GetUID() == XGetMyUID())
		postData.bAttackByMy = true;

	postData.nDamageFlags = pInfo->nDamageFlags;

	XEffectInvoker effectInvoker;
	effectInvoker.Invoke(pInfo->pOwner,pInfo->pEffectInfo->m_strEffectName, pInfo->pEffectInfo->m_strParameter, pInfo->pEffectInfo->m_strBoneName, &(pInfo->vecOutResultData), pInfo->pAttacker, &postData);
}

void XTalentEffect::CreateUseEffect( TALENT_EFFECT_INFO_DATA* pInfo )
{
	CreateCastEffect(pInfo);
}

void XTalentEffect::CreateAttackEffect( TALENT_EFFECT_INFO_DATA* pInfo )
{
	if (!pInfo->pOwner)
		return;

	// IMPACT_POS 확인
	if(pInfo->pEffectInfo->m_nImpactPos == HIT_POS)
	{
		if(CreateEnchantHitEffect(pInfo) == false)
			CreateCastEffect(pInfo);
	}
	else if(pInfo->pEffectInfo->m_nImpactPos == BONE_POS)
	{
		pInfo->effectPos = vec3::ZERO;
		pInfo->effectDir = vec3::AXISY;
		pInfo->matEffect.MakeIdentity();

		if(CreateEnchantHitEffect(pInfo) == false)
			CreateCastEffect(pInfo);
	}
	else // 모델
	{
		pInfo->effectPos = pInfo->pOwner->GetPosition();
		pInfo->effectDir = pInfo->pOwner->GetDirection();

		if(CreateEnchantHitEffect(pInfo) == false)
			CreateCastEffect(pInfo);
	}
}

void XTalentEffect::CreateMissEffect( TALENT_EFFECT_INFO_DATA* pInfo )
{
	CreateCastEffect(pInfo);
}

void XTalentEffect::CreateShockEffect( TALENT_EFFECT_INFO_DATA* pInfo )
{
	CreateCastEffect(pInfo);
}

void XTalentEffect::CreateDefenseEffect( TALENT_EFFECT_INFO_DATA* pInfo )
{
	CreateAttackEffect(pInfo);
}

void XTalentEffect::CreateDurationEffect( TALENT_EFFECT_INFO_DATA* pInfo )
{
	CreateCastEffect(pInfo);
}

void XTalentEffect::CreateAttackDefaultTalentEffect( CREATE_TALENT_EFFECT_DATA& creatTalentEffectData )
{
	if(creatTalentEffectData.pTalentInfo)
	{
		XTalentEffectBase * pEffectBase = NULL;

		if(creatTalentEffectData.pTalentInfo->m_nSkillType == ST_MELEE)
		{
			if(creatTalentEffectData.nType == ATTACK_SECONDARY_EFFECT)
				pEffectBase = info.talent_effect->GetTalentEffectBase(TEBY_MELEE_SECONDARY_ATTACK);
			else
				pEffectBase = info.talent_effect->GetTalentEffectBase(TEBY_MELEE_ATTACK);
		}
		else if(creatTalentEffectData.pTalentInfo->m_nSkillType == ST_MAGIC)
			pEffectBase = info.talent_effect->GetTalentEffectBase(TEBY_MAGIC_ATTACK);
		else if(creatTalentEffectData.pTalentInfo->m_nSkillType == ST_ARCHERY)
			pEffectBase = info.talent_effect->GetTalentEffectBase(TEBY_ARCHERY_ATTACK);
		else if(creatTalentEffectData.pTalentInfo->m_nSkillType == ST_GUN)
			pEffectBase = info.talent_effect->GetTalentEffectBase(TEBY_GUN_ATTACK);

		if(pEffectBase)
		{
			XTalentEffectInfo pEffectAttInfo;
			pEffectAttInfo.m_strEffectName	= pEffectBase->m_strEffectBaseFileName;
			pEffectAttInfo.m_strBoneName	= pEffectBase->m_strBoneName;

			pEffectAttInfo.m_nImpactPos		= pEffectBase->m_nImpactPos;

			//mlog("피격 디폴트로!!! %s\n", pEffectAttInfo.m_strEffectName.c_str());
			vec3 vDir = vec3(0.0f, 1.0f, 0.0f);
			if(creatTalentEffectData.pAttacker &&
				pEffectBase->m_nEffectDirection == TED_FRONT_EFFECT_USER)
			{
				vDir = creatTalentEffectData.pAttacker->GetDirection();
			}

			TALENT_EFFECT_INFO_DATA talentEffectInfoData;
			talentEffectInfoData.Set(&pEffectAttInfo, creatTalentEffectData.pOwner, creatTalentEffectData.effectPos, vDir, creatTalentEffectData.matEffect, creatTalentEffectData.pAttacker, creatTalentEffectData.nDamageFlags);
			return CreateAttackEffect(&talentEffectInfoData);
		}
	}
	else
	{
		// 탤런트가 없다면...
		XCharacter* pNode = creatTalentEffectData.pOwner->GetActor();
		if (pNode == NULL)
		{
			return;
		}

		global.emgr->AddXEffect(DEFAULT_EFFECT_FILE_NAME, pNode, NULL);
	}

	return;
}

void XTalentEffect::CreateDefenseDefaultTalentEffect(XObject* pObj, vec3 pos , XObject* pOtherObj /* = NULL */)
{
	if(pObj)
	{
		TALENT_EFFECT_BASE_TYPE effectType = TEBY_DEFENSE;

		if(pObj->GetEntityType() == ETID_NPC)
			effectType = TEBY_MONSTER_DEFENSE;
		else if(pObj->GetEntityType() == ETID_PLAYER)
		{
			XPlayer* player = static_cast<XPlayer*>(pObj);
			if(player->IsEquipShield() == false)
				effectType = TEBY_NOSHIELD_DEFENSE;
		}

		XTalentEffectBase * pEffectBase = info.talent_effect->GetTalentEffectBase(effectType);

		if(pEffectBase)
		{
			XTalentEffectInfo pEffectAttInfo;
			pEffectAttInfo.m_strEffectName	= pEffectBase->m_strEffectBaseFileName;
			pEffectAttInfo.m_strBoneName	= pEffectBase->m_strBoneName;

			pEffectAttInfo.m_nImpactPos		= pEffectBase->m_nImpactPos;

			//mlog("방어 디폴트로!!! %s\n", pEffectAttInfo.m_strEffectName.c_str());
			vec3 vDir = vec3(0.0f, 1.0f, 0.0f);
			if(pOtherObj &&
				pEffectBase->m_nEffectDirection == TED_FRONT_EFFECT_USER)
			{
				vDir = pOtherObj->GetDirection();
			}

			TALENT_EFFECT_INFO_DATA talentEffectInfoData;
			talentEffectInfoData.Set(&pEffectAttInfo, pObj, pos, vDir, MMatrix::IDENTITY, NULL, 0);
			return CreateDefenseEffect(&talentEffectInfoData);
		}
	}

	return;
}

vec3 XTalentEffect::GetTalentEffectDir( XTalentEffectInfo* pTalentEffectInfo, XObject* pObj_1, XObject* pObj_2 )
{
	vec3 vDir = vec3::AXISY;
	switch(pTalentEffectInfo->m_nEffectType)
	{
	case CAST_EFFECT:			// 캐스트
	case USE_EFFECT:			// 사용
		{
			//////////////////////////////////////////////////////////////////////////
			// by pok.
			// CreateFollowableEffect 에서 처리해주게 변경
			//////////////////////////////////////////////////////////////////////////
			// pObj_1이 사용자이고, pObj_2가 상대방이라면...
			//if(pObj_1 &&
			//	pTalentEffectInfo->m_nEffectDirection == TED_FRONT_EFFECT_USER)
			//{
			//	vDir = pObj_1->GetDirection();
			//}
		}
		break;
	case DEFENSE_EFFECT:
	case ATTACK_EFFECT:			// 피격
		{
			// pObj_1이 상대방이고, pObj_2가 사용자라면...
			if(pObj_2 &&
				pTalentEffectInfo->m_nEffectDirection == TED_FRONT_EFFECT_USER)
			{
				vDir = pObj_2->GetDirection();
			}
		}
		break;
	case MISS_EFFECT:			// 실패
	case SHOCK_EFFECT:			// 충격
	case DURATION_EFFECT:		// 지속
		break;
	}

	return vDir;
}

void XTalentEffect::CreateAttackBloodEffect( XObject* pObj, vec3 pos, vec3 dir, bool bCri, float fHitBloodScale, MMatrix matEffect )
{
	// 네오위즈 요청에 현재 피 보이는거 막은 상태입니다.
	return;

	if(gg.game->Check18Over() == false)
		return;

	// 피튀기는 이펙트 이름 가져오기
	wchar_t chEffectName[64]	= {0,};
	if(pObj->GetEntityType() != ETID_NPC)
	{
		if(XCONST::HIT_BASE_BLOOD_EFFECT_NAME == tstring(L""))
			return;

		swprintf(chEffectName, L"%s_%02d", XCONST::HIT_BASE_BLOOD_EFFECT_NAME, MMath::RandomNumber(1, BLOOD_EFFECT_COUNT));

		return;
	}
	else
	{
		XNonPlayer* pNonObj		= AsNPC(pObj);
		if(pNonObj && pNonObj->GetInfo() && pNonObj->GetInfo()->strBloodEffectName.empty() == false)
		{
			swprintf(chEffectName, L"%s_%02d", pNonObj->GetInfo()->strBloodEffectName.c_str(), MMath::RandomNumber(1, BLOOD_EFFECT_COUNT));
		}
		else
		{
			// 나오면 안되다고 해서...
			return;
			//// 위조건이 안돼면 디폴트로 보여준다.
			//sprintf(chEffectName, "ef_spread_blood_%02d", MMath::RandomNumber(1, 3));
		}
	}

	//------------------------------------------------------------------------
	// 피튀기는 이펙트
	// 회전 난수값
	int nxRotation	= MMath::RandomNumber(-90, -40);
	int nzRotation	= MMath::RandomNumber(-45, 45);
	vec3 vRanDir	= vec3(0.0f, -1.0f, 0.0f);
	vRanDir.Normalize();

	// 가까운 본의 좌표 찾기
	// 큰 NPC만 한다.
	bool bNearBone			= false;
	vec3 vBloodPos			= pos;
	RActorNode * pNearBone	= NULL;
	if(pObj->GetModuleCollision() &&
		pObj->GetActor() &&
		pObj->GetActor()->GetRadius() >= 100.0f)
	{
		pObj->GetModuleCollision()->PickRayFindNearBone(pos, &pNearBone);

		if(pNearBone)
			bNearBone = true;
	}
	
	if(bNearBone == false)
	{
		// 작은 NPC는 중심 본으로 한다.
		pNearBone = pObj->GetActor()->GetActorNode("Bip01");
	}

	// 매트릭스 셋팅
	RMatrix matDir, matBaseFrame;

	matDir = matEffect;

	MMatrix matInverse;
	pObj->GetWorldTransform().GetInverse(&matInverse);
	//matInverse.SetTranslation(vec3::ZERO);

	// 이펙트 생성	
	MUID uidEffect;
	uidEffect.SetZero();

	if(pNearBone)
	{
		matDir.SetTranslation(vBloodPos);
		if(matDir != MMatrix::IDENTITY)
			matDir *= matInverse;

		uidEffect = global.emgr->AddXEffect(chEffectName, pObj->GetActor(), NULL, matDir, false, true);
	}
	else
	{
		matBaseFrame.SetLocalMatrix(vBloodPos, dir, vec3::AXISZ);
		uidEffect = global.emgr->AddXEffect(chEffectName, NULL, NULL, matBaseFrame, false, false);
	}

	// 크리티컬은 두배!!!
	if(uidEffect != MUID::ZERO)
	{
		if(bCri)
			global.emgr->SetScaleXEffect(uidEffect, false, XCONST::HIT_CRITICAL_BLOOD_EFFECT_SIZE);
		else if(fHitBloodScale > 1.0f)
			global.emgr->SetScaleXEffect(uidEffect, false, fHitBloodScale);
	}
	//#ifdef _DEBUG
		//REngine::GetSceneManager().GetPassRenderer< RDebugPassRenderer >()->AddDebugBoxAndAxis(matDir, 30, 7, D3DCOLOR_XRGB(255, 0, 0));
	//#endif

	return;
}

void XTalentEffect::ClearEffect()
{
	for(vector<EFFECT_DEL>::iterator it = m_vecDelEffectList.begin(); it != m_vecDelEffectList.end(); ++it)
	{
		global.emgr->DeleteXEffect((*it).uidEffect);
	}

	for(vector<MUID>::iterator itSound = m_vecDelSoundList.begin(); itSound != m_vecDelSoundList.end(); ++itSound)
	{
		global.sound->StopSound(*itSound);
	}

	m_vecDelEffectList.clear();

	talentEffectHitCapsule.ClearAllTalentHitCapsuleImpact();
}

void XTalentEffect::Update( float fDelta )
{
	PFC_THISFUNC;
	talentEffectHitCapsule.Update(fDelta);
}

void XTalentEffect::CheckAttackEffectSound( vector<EFFECT_RESULT_DATA>& vecEffectResult )
{
	for(vector<EFFECT_RESULT_DATA>::iterator it = vecEffectResult.begin(); it != vecEffectResult.end(); it++)
	{
		if(it->type == CET_SOUND)
			m_bHaveAttackSound = true;
	}
}

void XTalentEffect::CreateCastEndEffect( TALENT_EFFECT_INFO_DATA* pInfo )
{
	CreateCastEffect(pInfo);
}

bool XTalentEffect::CreateEnchantHitEffect( TALENT_EFFECT_INFO_DATA* pInfo )
{
	if(pInfo->pAttacker == NULL || pInfo->pAttacker->GetEntityType() != ETID_PLAYER ||
		XEffectBaseInvoker::CheckReservedEffect(pInfo->pEffectInfo->m_strEffectName))			// 예약된건... 하면 안된다.
		return false;

	XPlayer* pPlayer = pInfo->pAttacker->AsPlayer();
	int nBuffID = pPlayer->GetVisualEnchantBuffID();

	// 버프 정보 가져오기
	XBuffInfo * pBuffInfo = info.buff->Get(nBuffID);
	if(pBuffInfo == NULL)
		return false;

	// 강화 버프 이펙트 정보 가져오기
	XActor* pXActor = pInfo->pAttacker->AsActor();
	WEAPON_TYPE eRType = pXActor->GetEquipWeaponType(false);
	WEAPON_TYPE eLType = pXActor->GetEquipWeaponType(true);

	vector<XEnchantBuffEffectInfo *> vecEnchantEffectInfoByType;
	pBuffInfo->m_EnchantBuffEffect.GetEffectInfoList(EBET_HIT_EFFECT, eRType, eLType, PLAYER_PARTS_SLOT_RWEAPON, vecEnchantEffectInfoByType);

	for(vector<XEnchantBuffEffectInfo *>::iterator it = vecEnchantEffectInfoByType.begin(); it != vecEnchantEffectInfoByType.end(); ++it)
	{
		XTalentEffectInfo talentEffectInfo;
		talentEffectInfo.copyData(*pInfo->pEffectInfo);
		talentEffectInfo.m_strEffectName = (*it)->m_strEffectName;

		pInfo->pEffectInfo = &talentEffectInfo;
		CreateCastEffect(pInfo);
	}

	return (vecEnchantEffectInfoByType.size() > 0);
}